#pragma once

#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace nundertale {

enum class CutsceneStepType {
    Wait,
    Dialog,
    Sound,
    SetFlag,
    Shake,
    Flash,
    StartBattle,
    Unsupported,
    Unknown
};

struct CutsceneStep {
    CutsceneStepType type = CutsceneStepType::Unknown;
    std::string typeName;
    float seconds = 0.0f;
    std::string speaker;
    std::vector<std::string> lines;
    std::string path;
    float volume = 1.0f;
    std::string key;
    int value = 0;
    float intensity = 0.0f;
    int r = 255;
    int g = 255;
    int b = 255;
    std::string encounter = "barky";
};

struct Cutscene {
    std::vector<CutsceneStep> steps;
};

inline CutsceneStepType cutsceneStepTypeFromString(const std::string& type) {
    if (type == "wait") return CutsceneStepType::Wait;
    if (type == "dialog") return CutsceneStepType::Dialog;
    if (type == "sound") return CutsceneStepType::Sound;
    if (type == "setFlag") return CutsceneStepType::SetFlag;
    if (type == "shake") return CutsceneStepType::Shake;
    if (type == "flash") return CutsceneStepType::Flash;
    if (type == "startBattle") return CutsceneStepType::StartBattle;
    if (type == "moveNpc" || type == "cameraPan" || type == "fadeOut") {
        return CutsceneStepType::Unsupported;
    }
    return CutsceneStepType::Unknown;
}

class CutsceneJsonParser {
public:
    explicit CutsceneJsonParser(const std::string& text) : m_text(text) {}

    bool parse(Cutscene& cutscene) {
        Cutscene parsed;
        skipWhitespace();
        if (!consume('{')) return false;

        bool foundSteps = false;
        skipWhitespace();
        if (!consume('}')) {
            while (true) {
                std::string key;
                if (!parseString(key)) return false;
                skipWhitespace();
                if (!consume(':')) return false;
                skipWhitespace();

                if (key == "steps") {
                    if (!parseSteps(parsed.steps)) return false;
                    foundSteps = true;
                } else if (!skipValue()) {
                    return false;
                }

                skipWhitespace();
                if (consume('}')) break;
                if (!consume(',')) return false;
                skipWhitespace();
            }
        }

        skipWhitespace();
        if (!atEnd() || !foundSteps) return false;
        cutscene = std::move(parsed);
        return true;
    }

private:
    bool parseSteps(std::vector<CutsceneStep>& steps) {
        if (!consume('[')) return false;
        skipWhitespace();
        if (consume(']')) return true;

        while (true) {
            CutsceneStep step;
            if (!parseStep(step)) return false;
            steps.push_back(std::move(step));

            skipWhitespace();
            if (consume(']')) return true;
            if (!consume(',')) return false;
            skipWhitespace();
        }
    }

    bool parseStep(CutsceneStep& step) {
        if (!consume('{')) return false;
        skipWhitespace();
        if (!consume('}')) {
            while (true) {
                std::string key;
                if (!parseString(key)) return false;
                skipWhitespace();
                if (!consume(':')) return false;
                skipWhitespace();

                if (!parseStepField(key, step)) return false;

                skipWhitespace();
                if (consume('}')) break;
                if (!consume(',')) return false;
                skipWhitespace();
            }
        }

        step.type = cutsceneStepTypeFromString(step.typeName);
        return true;
    }

    bool parseStepField(const std::string& key, CutsceneStep& step) {
        if (key == "type") return parseString(step.typeName);
        if (key == "speaker") return parseString(step.speaker);
        if (key == "lines") return parseStringArray(step.lines);
        if (key == "path") return parseString(step.path);
        if (key == "key") return parseString(step.key);
        if (key == "encounter" || key == "battle" || key == "id") {
            return parseString(step.encounter);
        }

        if (key == "seconds") return parseNumber(step.seconds);
        if (key == "volume") return parseNumber(step.volume);
        if (key == "intensity") return parseNumber(step.intensity);

        if (key == "value") return parseInteger(step.value);
        if (key == "r") return parseInteger(step.r);
        if (key == "g") return parseInteger(step.g);
        if (key == "b") return parseInteger(step.b);

        return skipValue();
    }

    bool parseStringArray(std::vector<std::string>& values) {
        if (!consume('[')) return false;
        skipWhitespace();
        if (consume(']')) return true;

        while (true) {
            std::string value;
            if (!parseString(value)) return false;
            values.push_back(std::move(value));

            skipWhitespace();
            if (consume(']')) return true;
            if (!consume(',')) return false;
            skipWhitespace();
        }
    }

    bool parseString(std::string& value) {
        skipWhitespace();
        if (!consume('"')) return false;
        value.clear();

        while (!atEnd()) {
            const char ch = m_text[m_pos++];
            if (ch == '"') return true;
            if (ch != '\\') {
                value.push_back(ch);
                continue;
            }
            if (atEnd()) return false;

            const char escaped = m_text[m_pos++];
            switch (escaped) {
                case '"': value.push_back('"'); break;
                case '\\': value.push_back('\\'); break;
                case '/': value.push_back('/'); break;
                case 'b': value.push_back('\b'); break;
                case 'f': value.push_back('\f'); break;
                case 'n': value.push_back('\n'); break;
                case 'r': value.push_back('\r'); break;
                case 't': value.push_back('\t'); break;
                default: return false;
            }
        }
        return false;
    }

    bool parseNumber(float& value) {
        skipWhitespace();
        if (atEnd()) return false;
        const char* begin = m_text.c_str() + m_pos;
        char* end = nullptr;
        value = std::strtof(begin, &end);
        if (end == begin) return false;
        m_pos = static_cast<std::size_t>(end - m_text.c_str());
        return true;
    }

    bool parseInteger(int& value) {
        float parsed = 0.0f;
        if (!parseNumber(parsed)) return false;
        value = static_cast<int>(parsed);
        return true;
    }

    bool skipValue() {
        skipWhitespace();
        if (atEnd()) return false;

        if (m_text[m_pos] == '"') {
            std::string ignored;
            return parseString(ignored);
        }
        if (m_text[m_pos] == '{') return skipObject();
        if (m_text[m_pos] == '[') return skipArray();

        const std::size_t start = m_pos;
        while (!atEnd() && m_text[m_pos] != ',' && m_text[m_pos] != '}' &&
               m_text[m_pos] != ']' && std::isspace(static_cast<unsigned char>(m_text[m_pos])) == 0) {
            ++m_pos;
        }
        return m_pos > start;
    }

    bool skipObject() {
        if (!consume('{')) return false;
        skipWhitespace();
        if (consume('}')) return true;
        while (true) {
            std::string ignoredKey;
            if (!parseString(ignoredKey)) return false;
            skipWhitespace();
            if (!consume(':')) return false;
            if (!skipValue()) return false;
            skipWhitespace();
            if (consume('}')) return true;
            if (!consume(',')) return false;
            skipWhitespace();
        }
    }

    bool skipArray() {
        if (!consume('[')) return false;
        skipWhitespace();
        if (consume(']')) return true;
        while (true) {
            if (!skipValue()) return false;
            skipWhitespace();
            if (consume(']')) return true;
            if (!consume(',')) return false;
            skipWhitespace();
        }
    }

    void skipWhitespace() {
        while (!atEnd() && std::isspace(static_cast<unsigned char>(m_text[m_pos])) != 0) {
            ++m_pos;
        }
    }

    bool consume(char expected) {
        if (!atEnd() && m_text[m_pos] == expected) {
            ++m_pos;
            return true;
        }
        return false;
    }

    bool atEnd() const { return m_pos >= m_text.size(); }

    const std::string& m_text;
    std::size_t m_pos = 0;
};

inline bool parseCutsceneJson(const std::string& text, Cutscene& cutscene) {
    return CutsceneJsonParser(text).parse(cutscene);
}

class CutsceneHost {
public:
    virtual ~CutsceneHost() = default;
    virtual void dialog(const std::string& speaker, const std::vector<std::string>& lines) = 0;
    virtual bool dialogOpen() const = 0;
    virtual void sound(const std::string& path, float volume) = 0;
    virtual void setFlag(const std::string& key, int value) = 0;
    virtual void shake(float intensity, float seconds) = 0;
    virtual void flash(int r, int g, int b, float seconds) = 0;
    virtual void startBattle(const std::string& encounter) = 0;
};

class CutsceneRunner {
public:
    void start(Cutscene cutscene) {
        m_cutscene = std::move(cutscene);
        m_index = 0;
        m_elapsed = 0.0f;
        m_stepStarted = false;
        m_running = !m_cutscene.steps.empty();
    }

    void update(float dt, CutsceneHost& host) {
        if (!m_running) return;
        float remainingDt = dt > 0.0f ? dt : 0.0f;

        while (m_running) {
            const CutsceneStep& step = m_cutscene.steps[m_index];
            switch (step.type) {
                case CutsceneStepType::Wait: {
                    if (step.seconds <= 0.0f) {
                        finishStep();
                        break;
                    }
                    const float needed = step.seconds - m_elapsed;
                    if (remainingDt < needed) {
                        m_elapsed += remainingDt;
                        return;
                    }
                    remainingDt -= needed;
                    finishStep();
                    break;
                }
                case CutsceneStepType::Dialog:
                    if (!m_stepStarted) {
                        m_stepStarted = true;
                        host.dialog(step.speaker, step.lines);
                        return;
                    }
                    if (host.dialogOpen()) return;
                    finishStep();
                    break;
                case CutsceneStepType::Sound:
                    host.sound(step.path, step.volume);
                    finishStep();
                    break;
                case CutsceneStepType::SetFlag:
                    host.setFlag(step.key, step.value);
                    finishStep();
                    break;
                case CutsceneStepType::Shake:
                    host.shake(step.intensity, step.seconds);
                    finishStep();
                    break;
                case CutsceneStepType::Flash:
                    host.flash(step.r, step.g, step.b, step.seconds);
                    finishStep();
                    break;
                case CutsceneStepType::StartBattle:
                    host.startBattle(step.encounter);
                    m_index = m_cutscene.steps.size();
                    m_running = false;
                    return;
                case CutsceneStepType::Unsupported:
                    std::clog << "Cutscene step '" << step.typeName
                              << "' not supported yet; skipping.\n";
                    finishStep();
                    break;
                case CutsceneStepType::Unknown:
                    finishStep();
                    break;
            }
        }
    }

    bool running() const { return m_running; }

private:
    void finishStep() {
        ++m_index;
        m_elapsed = 0.0f;
        m_stepStarted = false;
        if (m_index >= m_cutscene.steps.size()) m_running = false;
    }

    Cutscene m_cutscene;
    std::size_t m_index = 0;
    float m_elapsed = 0.0f;
    bool m_stepStarted = false;
    bool m_running = false;
};

} // namespace nundertale
