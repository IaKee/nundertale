#include "tools/ContentLint.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <system_error>
#include <vector>

namespace {

namespace fs = std::filesystem;

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

} // namespace

int main(int argc, char* argv[]) {
    if (argc > 2) {
        std::cerr << "Usage: content_lint [repository-root]" << std::endl;
        return 2;
    }

    const fs::path root = argc == 2 ? fs::path(argv[1]) : fs::path(".");
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

    std::size_t findingCount = 0U;
    for (const fs::path& path : files) {
        std::string contents;
        if (!readFile(path, contents)) {
            std::cerr << "Content lint error: could not read " << displayPath(root, path) << '.'
                      << std::endl;
            return 2;
        }
        const std::vector<nundertale::LintFinding> findings =
            nundertale::lintText(contents, blocked);
        for (const nundertale::LintFinding& finding : findings) {
            std::cout << displayPath(root, path) << ':' << finding.line << ':' << finding.col
                      << ": " << finding.rule << ": " << finding.snippet << std::endl;
            ++findingCount;
        }
    }

    if (findingCount == 0U) {
        std::cout << "Content lint passed: no findings in " << files.size() << " files."
                  << std::endl;
        return 0;
    }
    std::cout << "Content lint found " << findingCount << " issue(s) in " << files.size()
              << " files." << std::endl;
    return 1;
}
