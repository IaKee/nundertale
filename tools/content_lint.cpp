#include "tools/ContentLint.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

namespace {

namespace fs = std::filesystem;

struct TargetTerm {
    std::string rule;
    std::vector<std::string> words;
};

struct Issue {
    nundertale::LintFinding finding;
    bool isWarning = false;
};

bool readFile(const fs::path& path, std::string& contents) {
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        return false;
    }
    std::ostringstream buffer;
    buffer << input.rdbuf();
    if (input.bad()) {
        return false;
    }
    contents = buffer.str();
    return true;
}

bool collectJsonFiles(const fs::path& directory,
                      bool recursive,
                      std::vector<fs::path>& files) {
    std::error_code error;
    if (!fs::exists(directory, error)) {
        return !error;
    }
    if (error || !fs::is_directory(directory, error) || error) {
        return false;
    }

    if (recursive) {
        fs::recursive_directory_iterator iterator(directory, error);
        const fs::recursive_directory_iterator end;
        while (!error && iterator != end) {
            if (iterator->is_regular_file(error) && !error &&
                iterator->path().extension() == ".json") {
                files.push_back(iterator->path());
            }
            iterator.increment(error);
        }
    } else {
        fs::directory_iterator iterator(directory, error);
        const fs::directory_iterator end;
        while (!error && iterator != end) {
            if (iterator->is_regular_file(error) && !error &&
                iterator->path().extension() == ".json") {
                files.push_back(iterator->path());
            }
            iterator.increment(error);
        }
    }
    return !error;
}

bool loadBlocklist(const fs::path& path, std::vector<std::string>& terms) {
    std::error_code error;
    if (!fs::exists(path, error)) {
        return !error;
    }
    if (error) {
        return false;
    }

    std::ifstream input(path);
    if (!input) {
        return false;
    }
    std::string line;
    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        terms.push_back(line);
    }
    return !input.bad();
}

std::string displayPath(const fs::path& root, const fs::path& path) {
    std::error_code error;
    const fs::path relative = fs::relative(path, root, error);
    return (error ? path : relative).generic_string();
}

bool isAllowlistedFile(const std::string& path) {
    std::string p = path;
    std::replace(p.begin(), p.end(), '\\', '/');
    if (p.rfind("docs/", 0) == 0 || p.find("/docs/") != std::string::npos) {
        return true;
    }
    if (p == "README.md" || p == "ROTEIRO.md" || p == "BACKLOG.md") {
        return true;
    }
    if (p.find("data/flags.md") != std::string::npos) {
        return true;
    }
    if (p.find("PLACEHOLDERS.md") != std::string::npos) {
        return true;
    }
    if (p.find("assets/sprites/README.md") != std::string::npos) {
        return true;
    }
    return false;
}

bool isPlayerVisibleClean(const std::string& rawPath, const std::string& snippet, const std::string& rule) {
    (void)rule;
    std::string p = rawPath;
    std::replace(p.begin(), p.end(), '\\', '/');

    // Cleaned tree directories after T04/T14/T20/T21/NAR-032:
    // data/cutscenes/, data/enemies/, data/encounters/, data/dialogue/, data/items/
    const bool inCleanDir = (p.find("data/cutscenes/") != std::string::npos ||
                             p.find("data/enemies/") != std::string::npos ||
                             p.find("data/encounters/") != std::string::npos ||
                             p.find("data/dialogue/") != std::string::npos ||
                             p.find("data/items/") != std::string::npos);
    if (!inCleanDir) {
        return false;
    }

    // In data/items/: "notes": is developer metadata, not player-visible
    if (p.find("data/items/") != std::string::npos) {
        if (snippet.find("\"notes\"") != std::string::npos) {
            return false;
        }
    }

    // In cutscenes: "path":, "flag":, "sound":, "sprite": are internal metadata/assets
    if (p.find("data/cutscenes/") != std::string::npos) {
        if (snippet.find("\"path\"") != std::string::npos ||
            snippet.find("\"flag\"") != std::string::npos ||
            snippet.find("\"sound\"") != std::string::npos ||
            snippet.find("\"sprite\"") != std::string::npos) {
            return false;
        }
    }

    return true;
}

void scanForV3Terms(const std::string& text,
                    const std::vector<nundertale::content_lint_detail::PositionedCodePoint>& characters,
                    const std::vector<TargetTerm>& targets,
                    std::vector<nundertale::LintFinding>& findings) {
    using namespace nundertale::content_lint_detail;

    for (const auto& target : targets) {
        std::vector<std::vector<char32_t>> foldedWords;
        for (const auto& w : target.words) {
            foldedWords.push_back(foldedCodePoints(w));
        }
        if (foldedWords.empty()) {
            continue;
        }

        for (std::size_t index = 0U; index < characters.size(); ++index) {
            // Left boundary check
            const bool leftBoundary = (index == 0U) || !isWordCharacter(characters[index - 1U].value);
            if (!leftBoundary) {
                continue;
            }

            if (!matchesAt(characters, index, foldedWords[0])) {
                continue;
            }

            std::size_t curr = index + foldedWords[0].size();
            bool fullMatch = true;

            for (std::size_t w = 1U; w < foldedWords.size(); ++w) {
                std::size_t spaceCount = 0U;
                while (curr < characters.size() &&
                       (characters[curr].value == U' ' || characters[curr].value == U'\t' ||
                        characters[curr].value == U'\n' || characters[curr].value == U'\r')) {
                    ++curr;
                    ++spaceCount;
                }
                if (spaceCount == 0U || curr >= characters.size()) {
                    fullMatch = false;
                    break;
                }
                if (!matchesAt(characters, curr, foldedWords[w])) {
                    fullMatch = false;
                    break;
                }
                curr += foldedWords[w].size();
            }

            if (!fullMatch) {
                continue;
            }

            // Right boundary check
            const bool rightBoundary = (curr == characters.size()) || !isWordCharacter(characters[curr].value);
            if (!rightBoundary) {
                continue;
            }

            const std::string snippet = lineSnippet(text, characters[index].line);

            // Ignore flag identifiers for raid (e.g. raid.time_left is canonical flag prefix)
            if (target.rule == "raid-word") {
                if (snippet.find("\"flag\"") != std::string::npos ||
                    snippet.find("showIf") != std::string::npos ||
                    snippet.find("hideIf") != std::string::npos) {
                    continue;
                }
            }

            findings.push_back({characters[index].line, characters[index].col, target.rule, snippet});
        }
    }
}

} // namespace

int main(int argc, char* argv[]) {
    bool checkJson = true;
    bool warningsAsErrors = false;
    fs::path root = ".";
    bool rootSet = false;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--json") {
            checkJson = true;
        } else if (arg == "--no-json") {
            checkJson = false;
        } else if (arg == "--warnings-as-errors") {
            warningsAsErrors = true;
        } else if (arg.rfind("--", 0) == 0) {
            std::cerr << "Usage: content_lint [--json] [--no-json] [--warnings-as-errors] [repository-root]" << std::endl;
            return 2;
        } else {
            if (rootSet) {
                std::cerr << "Usage: content_lint [--json] [--no-json] [--warnings-as-errors] [repository-root]" << std::endl;
                return 2;
            }
            root = fs::path(arg);
            rootSet = true;
        }
    }

    std::error_code error;
    if (!fs::is_directory(root, error) || error) {
        std::cerr << "Content lint error: repository root is not a readable directory." << std::endl;
        return 2;
    }

    std::vector<std::string> blocked;
    if (!loadBlocklist(root / "tools" / "content_lint_blocklist.txt", blocked)) {
        std::cerr << "Content lint error: could not read the blocklist." << std::endl;
        return 2;
    }

    std::vector<fs::path> files;
    if (!collectJsonFiles(root / "data", true, files) ||
        !collectJsonFiles(root / "assets" / "lang", false, files)) {
        std::cerr << "Content lint error: could not scan content directories." << std::endl;
        return 2;
    }
    std::sort(files.begin(), files.end());

    static const std::vector<TargetTerm> v3Targets = {
        {"my-child-phrase", {"my", "child"}},
        {"furry-over-phrase", {"furry", "over"}},
        {"raid-word", {"raid"}}
    };

    std::size_t errorCount = 0U;
    std::size_t warningCount = 0U;

    for (const fs::path& path : files) {
        const std::string normPath = displayPath(root, path);
        if (isAllowlistedFile(normPath)) {
            continue;
        }

        std::string contents;
        if (!readFile(path, contents)) {
            std::cerr << "Content lint error: could not read " << normPath << '.' << std::endl;
            return 2;
        }

        std::vector<nundertale::LintFinding> baseFindings =
            nundertale::lintText(contents, blocked, normPath);

        const auto chars = nundertale::content_lint_detail::decodePositionedCharacters(contents);
        scanForV3Terms(contents, chars, v3Targets, baseFindings);

        if (checkJson) {
            nundertale::LintFinding jsonFinding;
            if (!nundertale::validateJson(contents, &jsonFinding)) {
                baseFindings.push_back(jsonFinding);
            }
        }

        std::vector<Issue> issues;
        issues.reserve(baseFindings.size());
        for (const auto& finding : baseFindings) {
            bool isWarn = false;
            if (finding.rule == "pepe-word" ||
                finding.rule == "my-child-phrase" ||
                finding.rule == "furry-over-phrase" ||
                finding.rule == "raid-word") {
                if (!isPlayerVisibleClean(normPath, finding.snippet, finding.rule)) {
                    isWarn = true;
                }
            }
            if (warningsAsErrors) {
                isWarn = false;
            }
            issues.push_back({finding, isWarn});
        }

        std::sort(issues.begin(), issues.end(), [](const Issue& left, const Issue& right) {
            if (left.finding.line != right.finding.line) {
                return left.finding.line < right.finding.line;
            }
            if (left.finding.col != right.finding.col) {
                return left.finding.col < right.finding.col;
            }
            return left.finding.rule < right.finding.rule;
        });

        for (const Issue& issue : issues) {
            if (issue.isWarning) {
                std::cout << normPath << ':' << issue.finding.line << ':' << issue.finding.col
                          << ": warning: " << issue.finding.rule << ": " << issue.finding.snippet << std::endl;
                ++warningCount;
            } else {
                std::cout << normPath << ':' << issue.finding.line << ':' << issue.finding.col
                          << ": " << issue.finding.rule << ": " << issue.finding.snippet << std::endl;
                ++errorCount;
            }
        }
    }

    if (errorCount == 0U) {
        if (warningCount == 0U) {
            std::cout << "Content lint passed: no findings in " << files.size() << " files." << std::endl;
        } else {
            std::cout << "Content lint passed: 0 errors, " << warningCount << " warning(s) in "
                      << files.size() << " files." << std::endl;
        }
        return 0;
    }

    std::cout << "Content lint found " << errorCount << " error(s), " << warningCount << " warning(s) in "
              << files.size() << " files." << std::endl;
    return 1;
}
