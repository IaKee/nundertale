#pragma once

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "core/SessionMeta.hpp"
#include "core/ToxicTrack.hpp"

namespace nundertale {

enum class CutsceneStepType {
    Wait,
    Dialog,
    Sound,
    SetFlag,
    Shake,
    Flash,
    StartBattle,
    OpenShop,
    SetFur,
    HealFull,
    FacePlayer,
    PlayerAnim,
    GiveItem,
    AddFlag,
    AddFur,
    NpcFace,
    NpcAnim,
    HideNpc,
    ShowNpc,
    MoveNpc,
    MovePlayer,
    LockControls,
    FadeOut,
    FadeIn,
    Choice,
    RecordKill,
    PlayCutscene,
    WaterBurst,
    Unsupported,
    Unknown
};

struct CutsceneStep {
    CutsceneStepType type = CutsceneStepType::Unknown;
    std::string typeName;
    float seconds = 0.0f;
    std::string speaker;
    std::string portrait;
    std::string reveal;
    std::vector<std::string> lines;
    std::string path;
    float volume = 1.0f;
    std::string key;
    int value = 0;
    float intensity = 0.0f;
    int r = 255;
    int g = 255;
    int b = 255;
    std::string encounter;
    std::string shop;
    std::string condition;
    std::string item;
    std::vector<std::string> fullLines;
    std::vector<std::string> options;
    std::string npc;
    float x = 0.0f;
    float y = 0.0f;
    float speed = 80.0f;
    bool secondsSpecified = false;
};

struct Cutscene {
    std::string skipFlag;
    std::vector<CutsceneStep> steps;
};

// True when the cutscene hands the player over to a fight (a startBattle step).
inline bool cutsceneStartsBattle(const Cutscene& cutscene) {
    return std::any_of(cutscene.steps.begin(), cutscene.steps.end(), [](const CutsceneStep& step) {
        return step.type == CutsceneStepType::StartBattle;
    });
}

// A trigger's "once" flag is what keeps its cutscene from playing again. A cutscene that ends in a
// fight must not burn it until the fight really started: if the battle cannot be built, the flag
// stays free so the player meets the fight on the next visit instead of losing it for good.
inline bool triggerOnceShouldBurn(bool cutsceneHasBattle, bool battleStarted) {
    return !cutsceneHasBattle || battleStarted;
}

// Battle scripted ends leave this one-shot hook in GameState. The overworld takes ownership when
// it resumes so a later resume/save load cannot replay the same ending.
inline std::string takePendingCutscene(std::string& pending) {
    std::string result = std::move(pending);
    pending.clear();
    return result;
}

// C1-014: a "choice" step shows `lines` as the prompt and 2-3 options in the dialog box. The picked
// option is stored in the flag `key` as its 1-based number (0 stays "not chosen yet"); later steps
// branch on it with "if": "<key> == 1". Option text cannot hold the dialog box's '|' and '}' tokens.
inline constexpr std::size_t CUTSCENE_CHOICE_MIN_OPTIONS = 2;
inline constexpr std::size_t CUTSCENE_CHOICE_MAX_OPTIONS = 3;

inline bool cutsceneChoiceValid(const CutsceneStep& step) {
    if (step.options.size() < CUTSCENE_CHOICE_MIN_OPTIONS ||
        step.options.size() > CUTSCENE_CHOICE_MAX_OPTIONS) {
        return false;
    }
    for (const std::string& option : step.options) {
        if (option.empty() || option.find_first_of("|}") != std::string::npos) return false;
    }
    return true;
}

// The dialog pages of a choice step: the prompt pages, the last one carrying the option token.
inline std::vector<std::string> cutsceneChoicePages(const CutsceneStep& step) {
    std::vector<std::string> pages = step.lines;
    if (pages.empty()) pages.emplace_back();
    std::string token = "{choice:";
    for (std::size_t i = 0; i < step.options.size(); ++i) {
        if (i > 0) token += "|";
        token += step.options[i];
    }
    token += "}";
    pages.back() += token;
    return pages;
}

// C1-014: a kill that happens outside a battle (the Apurrture sacrifice) sets what a battle kill
// sets (BattleSystem.cpp): the enemy's killed flags, kills.ruins, the totals and the toxic track
// (which also loses the Clean Run: killsTotal > 0 / kills.total). Returns true when it started or
// fed the toxic track.
inline bool applyCutsceneKill(GameState& state, SessionMeta& session, const std::string& enemyId) {
    if (enemyId.empty()) return false;
    if (enemyId == "barky") {
        state.setFlag("ruins.barky.killed", 1);
        state.setFlag("ruins.barky.state", 2);
    }
    state.incFlag("battle." + enemyId + ".killed");
    state.incFlag("kills.ruins");
    state.killsTotal += 1;
    state.killsByArea["ruins"] += 1;
    state.setFlag("kills.total", state.killsTotal);
    sessionOnKill(session);
    return toxicTrackOnKill(state, enemyId);
}

inline CutsceneStepType cutsceneStepTypeFromString(const std::string& type) {
    if (type == "wait") return CutsceneStepType::Wait;
    if (type == "dialog") return CutsceneStepType::Dialog;
    if (type == "sound") return CutsceneStepType::Sound;
    if (type == "setFlag") return CutsceneStepType::SetFlag;
    if (type == "shake") return CutsceneStepType::Shake;
    if (type == "flash") return CutsceneStepType::Flash;
    if (type == "startBattle") return CutsceneStepType::StartBattle;
    if (type == "openShop") return CutsceneStepType::OpenShop;
    if (type == "setFur") return CutsceneStepType::SetFur;
    if (type == "healFull") return CutsceneStepType::HealFull;
    if (type == "facePlayer") return CutsceneStepType::FacePlayer;
    if (type == "playerAnim") return CutsceneStepType::PlayerAnim;
    if (type == "giveItem") return CutsceneStepType::GiveItem;
    if (type == "addFlag") return CutsceneStepType::AddFlag;
    if (type == "addFur") return CutsceneStepType::AddFur;
    if (type == "npcFace") return CutsceneStepType::NpcFace;
    if (type == "npcAnim") return CutsceneStepType::NpcAnim;
    if (type == "hideNpc") return CutsceneStepType::HideNpc;
    if (type == "showNpc") return CutsceneStepType::ShowNpc;
    if (type == "moveNpc") return CutsceneStepType::MoveNpc;
    if (type == "movePlayer") return CutsceneStepType::MovePlayer;
    if (type == "lockControls") return CutsceneStepType::LockControls;
    if (type == "fadeOut") return CutsceneStepType::FadeOut;
    if (type == "fadeIn") return CutsceneStepType::FadeIn;
    if (type == "choice") return CutsceneStepType::Choice;
    if (type == "recordKill") return CutsceneStepType::RecordKill;
    if (type == "cutscene") return CutsceneStepType::PlayCutscene;
    if (type == "waterBurst") return CutsceneStepType::WaterBurst;
    if (type == "cameraPan") {
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
                } else if (key == "skipFlag") {
                    if (!parseString(parsed.skipFlag)) return false;
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
        if ((step.type == CutsceneStepType::FadeOut || step.type == CutsceneStepType::FadeIn) &&
            !step.secondsSpecified) {
            step.seconds = 0.5f;
        }
        return true;
    }

    bool parseStepField(const std::string& key, CutsceneStep& step) {
        if (key == "type") return parseString(step.typeName);
        if (key == "speaker") return parseString(step.speaker);
        if (key == "portrait") return parseString(step.portrait);
        if (key == "reveal") return parseString(step.reveal);
        if (key == "lines") return parseStringArray(step.lines);
        if (key == "path") return parseString(step.path);
        if (key == "key") return parseString(step.key);
        if (key == "encounter") return parseString(step.encounter);
        if (key == "shop") return parseString(step.shop);
        if (key == "if") return parseString(step.condition);
        if (key == "item") return parseString(step.item);
        if (key == "fullLines") return parseStringArray(step.fullLines);
        if (key == "options") return parseStringArray(step.options);
        if (key == "npc" || key == "target") return parseString(step.npc);

        if (key == "seconds") {
            step.secondsSpecified = true;
            return parseNumber(step.seconds);
        }
        if (key == "volume") return parseNumber(step.volume);
        if (key == "intensity") return parseNumber(step.intensity);
        if (key == "x") return parseNumber(step.x);
        if (key == "y") return parseNumber(step.y);
        if (key == "speed") return parseNumber(step.speed);

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

inline float fadeAlpha(float start, float target, float elapsed, float seconds) {
    start = std::clamp(start, 0.0f, 1.0f);
    target = std::clamp(target, 0.0f, 1.0f);
    if (seconds <= 0.0f || elapsed >= seconds) return target;
    const float progress = std::clamp(elapsed / seconds, 0.0f, 1.0f);
    return start + (target - start) * progress;
}

class CutsceneHost {
public:
    virtual ~CutsceneHost() = default;
    virtual void dialog(const std::string& speaker, const std::vector<std::string>& lines) = 0;
    virtual void dialog(const std::string& speaker, const std::vector<std::string>& lines,
                        const std::string&) {
        dialog(speaker, lines);
    }
    virtual void dialog(const std::string& speaker, const std::vector<std::string>& lines,
                        const std::string& portrait, const std::string&) {
        dialog(speaker, lines, portrait);
    }
    virtual bool dialogOpen() const = 0;
    virtual void sound(const std::string& path, float volume) = 0;
    virtual void setFlag(const std::string& key, int value) = 0;
    virtual void shake(float intensity, float seconds) = 0;
    virtual void flash(int r, int g, int b, float seconds) = 0;
    virtual void startBattle(const std::string& encounter) = 0;
    virtual void openShop(const std::string&) {}
    virtual void setFur(int value) = 0;
    virtual void healFull() = 0;
    virtual void facePlayer(const std::string& key) = 0;
    virtual void playerAnim(const std::string& path, const std::string& key, float seconds) = 0;
    virtual bool condition(const std::string&) { return true; }
    virtual void giveItem(const std::string&, const std::vector<std::string>&,
                          const std::vector<std::string>&) {}
    virtual void addFlag(const std::string&, int) {}
    virtual void addFur(int) {}
    virtual void npcFace(const std::string&, const std::string&) {}
    virtual void npcAnim(const std::string&, const std::string&, float) {}
    virtual void setNpcVisible(const std::string&, bool) {}
    virtual bool moveNpc(const std::string&, float, float, float, float) { return true; }
    virtual bool movePlayer(float, float, float, float) { return true; }
    virtual void lockControls(float) {}
    virtual bool fadeOut(float, float) { return true; }
    virtual bool fadeIn(float, float) { return true; }
    // C1-014 "choice": beginChoice opens the prompt with its options, choiceResult returns the
    // 0-based pick or -1 while the player is still choosing (a host without UI picks the first).
    virtual void beginChoice(const std::string&, const std::vector<std::string>&,
                             const std::vector<std::string>&) {}
    virtual void beginChoice(const std::string& speaker, const std::vector<std::string>& pages,
                             const std::vector<std::string>& options, const std::string&) {
        beginChoice(speaker, pages, options);
    }
    virtual int choiceResult() { return 0; }
    // C1-014 "recordKill": the enemy id (step.encounter) dies outside a battle.
    virtual void recordKill(const std::string&) {}
    // C1-014 "cutscene": hands over to another cutscene (data/cutscenes/<key>.json).
    virtual bool loadCutscene(const std::string&, Cutscene&) { return false; }
    // C1-002: spawn the overworld spritz at "player" (default) or an NPC/prop name.
    virtual void waterBurst(const std::string&) {}
};

class CutsceneRunner {
public:
    const std::string& skipFlag() const { return m_cutscene.skipFlag; }
    void stop() {
        m_running = false;
        m_cutscene.steps.clear();
        m_index = 0;
        m_elapsed = 0.0f;
    }

    void start(Cutscene cutscene) {
        m_cutscene = std::move(cutscene);
        m_index = 0;
        m_elapsed = 0.0f;
        m_stepStarted = false;
        m_conditionChecked = false;
        m_running = !m_cutscene.steps.empty();
    }

    void update(float dt, CutsceneHost& host) {
        if (!m_running) return;
        float remainingDt = dt > 0.0f ? dt : 0.0f;

        while (m_running) {
            const CutsceneStep& step = m_cutscene.steps[m_index];
            if (!m_conditionChecked) {
                m_conditionChecked = true;
                if (!step.condition.empty() && !host.condition(step.condition)) {
                    finishStep();
                    continue;
                }
            }
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
                        host.dialog(step.speaker, step.lines, step.portrait, step.reveal);
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
                case CutsceneStepType::OpenShop:
                    if (!m_stepStarted) {
                        m_stepStarted = true;
                        host.openShop(step.shop);
                        return;
                    }
                    finishStep();
                    break;
                case CutsceneStepType::SetFur:
                    host.setFur(step.value);
                    finishStep();
                    break;
                case CutsceneStepType::HealFull:
                    host.healFull();
                    finishStep();
                    break;
                case CutsceneStepType::FacePlayer:
                    host.facePlayer(step.key);
                    finishStep();
                    break;
                case CutsceneStepType::PlayerAnim: {
                    if (!m_stepStarted) {
                        m_stepStarted = true;
                        host.playerAnim(step.path, step.key, step.seconds);
                    }
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
                case CutsceneStepType::GiveItem:
                    if (!m_stepStarted) {
                        m_stepStarted = true;
                        host.giveItem(step.item, step.lines, step.fullLines);
                        if (host.dialogOpen()) return;
                    } else if (host.dialogOpen()) {
                        return;
                    }
                    finishStep();
                    break;
                case CutsceneStepType::AddFlag:
                    host.addFlag(step.key, step.value);
                    finishStep();
                    break;
                case CutsceneStepType::AddFur:
                    host.addFur(step.value);
                    finishStep();
                    break;
                case CutsceneStepType::NpcFace:
                    host.npcFace(step.npc, step.key);
                    finishStep();
                    break;
                case CutsceneStepType::NpcAnim: {
                    if (!m_stepStarted) {
                        m_stepStarted = true;
                        host.npcAnim(step.npc, step.key, step.seconds);
                    }
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
                case CutsceneStepType::HideNpc:
                    host.setNpcVisible(step.npc, false);
                    finishStep();
                    break;
                case CutsceneStepType::ShowNpc:
                    host.setNpcVisible(step.npc, true);
                    finishStep();
                    break;
                case CutsceneStepType::MoveNpc:
                    if (!host.moveNpc(step.npc, step.x, step.y, step.speed, remainingDt)) return;
                    finishStep();
                    break;
                case CutsceneStepType::MovePlayer:
                    if (!host.movePlayer(step.x, step.y, step.speed, remainingDt)) return;
                    finishStep();
                    break;
                case CutsceneStepType::LockControls:
                    host.lockControls(step.seconds);
                    finishStep();
                    break;
                case CutsceneStepType::FadeOut:
                    if (!host.fadeOut(step.seconds, remainingDt)) return;
                    finishStep();
                    break;
                case CutsceneStepType::FadeIn:
                    if (!host.fadeIn(step.seconds, remainingDt)) return;
                    finishStep();
                    break;
                case CutsceneStepType::Choice: {
                    if (!cutsceneChoiceValid(step)) {
                        std::clog << "Cutscene choice step needs 2-3 plain options; skipping.\n";
                        finishStep();
                        break;
                    }
                    if (!m_stepStarted) {
                        m_stepStarted = true;
                        host.beginChoice(step.speaker, cutsceneChoicePages(step), step.options,
                                         step.portrait);
                    }
                    const int picked = host.choiceResult();
                    if (picked < 0) return;
                    const int count = static_cast<int>(step.options.size());
                    if (!step.key.empty()) host.setFlag(step.key, std::clamp(picked, 0, count - 1) + 1);
                    finishStep();
                    break;
                }
                case CutsceneStepType::RecordKill:
                    host.recordKill(step.encounter);
                    finishStep();
                    break;
                case CutsceneStepType::PlayCutscene: {
                    Cutscene next;
                    if (!host.loadCutscene(step.key, next) || next.steps.empty()) {
                        std::clog << "Cutscene '" << step.key << "' could not be loaded; skipping.\n";
                        finishStep();
                        break;
                    }
                    // `step` points into the cutscene being replaced: do not touch it after this.
                    m_cutscene = std::move(next);
                    m_index = 0;
                    m_elapsed = 0.0f;
                    m_stepStarted = false;
                    m_conditionChecked = false;
                    break;
                }
                case CutsceneStepType::WaterBurst:
                    host.waterBurst(step.npc);
                    finishStep();
                    break;
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
        m_conditionChecked = false;
        if (m_index >= m_cutscene.steps.size()) m_running = false;
    }

    Cutscene m_cutscene;
    std::size_t m_index = 0;
    float m_elapsed = 0.0f;
    bool m_stepStarted = false;
    bool m_conditionChecked = false;
    bool m_running = false;
};

} // namespace nundertale
