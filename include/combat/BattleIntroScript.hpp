#pragma once

#include "combat/FloofyScript.hpp"
#include "combat/FurHud.hpp"
#include "core/FloofyAnnoyance.hpp"
#include "core/TiledMap.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <deque>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace nundertale {

// LORE §19.0 steps 2-3 (beats 8-18): the in-box fake tutorial that runs before the first menu turn.
// Pure state machine, no engine calls: BattleScene feeds it input and draws what it exposes.
// The script is data (the "intro" block of data/encounters/<id>.json), parsed by parseIntroScript.

using IntroFlags = std::unordered_map<std::string, int>;

inline constexpr const char* INTRO_FLAG_ANNOYED = "ruins.floofy.annoyed";
inline constexpr const char* INTRO_FLAG_DODGES = "ruins.floofy.dodges";
inline constexpr const char* INTRO_FLAG_DEATHS = "ruins.floofy.deaths";
inline constexpr const char* INTRO_FLAG_FUR_SEEN = "fur_hud_seen";

enum class IntroSpeaker { Floofy, Narrator };

enum class IntroStepKind {
    Say,          // Floofy's speech bubble, one page per line
    Narrate,      // box text, one page per line
    Choice,       // two (or more) options, each with its own reply steps
    Petals,       // the tutorial_petals phase: dodge branches, then the forced hit
    StopMusic,    // the cute music stops (beat 14)
    Sound,        // one-shot SFX by id
    ShowPetals,   // beat 10: the petals appear around him, buzzing
    Flash,        // the sinister face flashes
    Pause,        // a timed pause (the long pause of beat 14)
    PickFight,    // beat 18: variant lines by ruins.floofy.annoyed, locks ruins.floofy.fight
    NextRound     // internal: after a dodge branch, the next petal round starts
};

struct IntroStep;

struct IntroChoiceOption {
    std::string label;
    int value = 0;     // stored in the choice flag
    int annoy = 0;     // added to ruins.floofy.annoyed
    std::vector<IntroStep> reply;
};

struct IntroPetalsDef {
    std::string pattern = "tutorial_petals";
    std::string forcedPattern = "forced_petal";
    int count = 5;
    float speed = 48.0f;           // px/s of the first round
    float retrySpeedScale = 0.6f;  // after one dodge: slower
    float fastSpeedScale = 2.4f;   // after two dodges: fast and undodgeable
    int annoySecondDodge = 1;
    int damagePct = 25;            // of max HP
    int fur = 20;
    int furCap = 99;               // no Mirror before R3
    std::vector<IntroStep> onDodgeOnce;
    std::vector<IntroStep> onDodgeTwice;
    std::vector<IntroStep> afterHit;
};

struct IntroStep {
    IntroStepKind kind = IntroStepKind::Say;
    int beat = 0;                       // LORE §19.0 beat number, 0 = none
    std::vector<std::string> lines;     // Say / Narrate pages
    std::string id;                     // Sound id, Flash kind
    float seconds = 0.0f;               // Pause / Flash length
    std::string flag;                   // Choice: flag that stores the option value
    std::vector<IntroChoiceOption> options;
    IntroPetalsDef petals;
    std::vector<IntroStep> petalsFight; // PickFight, variant 1
    std::vector<IntroStep> bassFight;   // PickFight, variant 2
};

struct IntroScriptDef {
    std::string music;                  // cute lounge music id for the intro
    std::string skipFlag = INTRO_FLAG_DEATHS;  // intro skipped when this flag is >= 1 (retry)
    std::vector<IntroStep> steps;
    bool empty() const { return steps.empty(); }
};

// ---------------------------------------------------------------- JSON parsing

namespace intro_detail {

using Json = tiled_detail::JsonValue;

inline std::string strField(const Json& v, const char* key) {
    const Json* f = v.field(key);
    return (f != nullptr && f->kind == Json::Kind::String) ? f->string : std::string();
}

inline double numField(const Json& v, const char* key, double fallback) {
    const Json* f = v.field(key);
    return (f != nullptr && f->kind == Json::Kind::Number) ? f->number : fallback;
}

inline bool parseSteps(const Json& arr, std::vector<IntroStep>& out);

inline bool parseStepList(const Json& v, const char* key, std::vector<IntroStep>& out) {
    const Json* f = v.field(key);
    if (f == nullptr) return true;
    if (f->kind != Json::Kind::Array) return false;
    return parseSteps(*f, out);
}

inline bool parseStep(const Json& v, IntroStep& step) {
    if (v.kind != Json::Kind::Object) return false;
    const std::string type = strField(v, "type");
    step.beat = static_cast<int>(numField(v, "beat", 0.0));
    step.id = strField(v, "id");
    step.seconds = static_cast<float>(numField(v, "seconds", 0.0));
    if (const Json* lines = v.field("lines"); lines != nullptr) {
        if (lines->kind != Json::Kind::Array) return false;
        for (const Json& l : lines->array) {
            if (l.kind != Json::Kind::String) return false;
            step.lines.push_back(l.string);
        }
    }
    if (type == "say") {
        step.kind = IntroStepKind::Say;
    } else if (type == "narrate") {
        step.kind = IntroStepKind::Narrate;
    } else if (type == "stop_music") {
        step.kind = IntroStepKind::StopMusic;
    } else if (type == "sound") {
        step.kind = IntroStepKind::Sound;
    } else if (type == "show_petals") {
        step.kind = IntroStepKind::ShowPetals;
    } else if (type == "flash") {
        step.kind = IntroStepKind::Flash;
        if (step.seconds <= 0.0f) step.seconds = 0.12f;
    } else if (type == "pause") {
        step.kind = IntroStepKind::Pause;
    } else if (type == "choice") {
        step.kind = IntroStepKind::Choice;
        step.flag = strField(v, "flag");
        const Json* opts = v.field("options");
        if (opts == nullptr || opts->kind != Json::Kind::Array || opts->array.size() < 2U) return false;
        for (const Json& o : opts->array) {
            if (o.kind != Json::Kind::Object) return false;
            IntroChoiceOption opt;
            opt.label = strField(o, "label");
            opt.value = static_cast<int>(numField(o, "value", 0.0));
            opt.annoy = static_cast<int>(numField(o, "annoy", 0.0));
            if (opt.label.empty() || !parseStepList(o, "reply", opt.reply)) return false;
            step.options.push_back(std::move(opt));
        }
    } else if (type == "petals") {
        step.kind = IntroStepKind::Petals;
        IntroPetalsDef& p = step.petals;
        if (const std::string s = strField(v, "pattern"); !s.empty()) p.pattern = s;
        if (const std::string s = strField(v, "forcedPattern"); !s.empty()) p.forcedPattern = s;
        p.count = static_cast<int>(numField(v, "count", p.count));
        p.speed = static_cast<float>(numField(v, "speed", static_cast<double>(p.speed)));
        p.retrySpeedScale = static_cast<float>(numField(v, "retrySpeedScale", static_cast<double>(p.retrySpeedScale)));
        p.fastSpeedScale = static_cast<float>(numField(v, "fastSpeedScale", static_cast<double>(p.fastSpeedScale)));
        p.annoySecondDodge = static_cast<int>(numField(v, "annoySecondDodge", p.annoySecondDodge));
        p.damagePct = static_cast<int>(numField(v, "damagePct", p.damagePct));
        p.fur = static_cast<int>(numField(v, "fur", p.fur));
        p.furCap = static_cast<int>(numField(v, "furCap", p.furCap));
        if (!parseStepList(v, "onDodgeOnce", p.onDodgeOnce) ||
            !parseStepList(v, "onDodgeTwice", p.onDodgeTwice) ||
            !parseStepList(v, "afterHit", p.afterHit)) return false;
    } else if (type == "pick_fight") {
        step.kind = IntroStepKind::PickFight;
        if (!parseStepList(v, "petals", step.petalsFight) ||
            !parseStepList(v, "bass", step.bassFight)) return false;
    } else {
        return false;
    }
    if ((step.kind == IntroStepKind::Say || step.kind == IntroStepKind::Narrate) && step.lines.empty()) {
        return false;
    }
    return true;
}

inline bool parseSteps(const Json& arr, std::vector<IntroStep>& out) {
    if (arr.kind != Json::Kind::Array) return false;
    for (const Json& item : arr.array) {
        IntroStep step;
        if (!parseStep(item, step)) return false;
        out.push_back(std::move(step));
    }
    return true;
}

}  // namespace intro_detail

// Parses the "intro" object of an encounter file. Returns false (and leaves `out` empty) on malformed data.
inline bool parseIntroScript(const tiled_detail::JsonValue& intro, IntroScriptDef& out) {
    using namespace intro_detail;
    IntroScriptDef def;
    if (intro.kind != Json::Kind::Object) return false;
    def.music = strField(intro, "music");
    if (const std::string s = strField(intro, "skipFlag"); !s.empty()) def.skipFlag = s;
    const Json* steps = intro.field("beats");
    if (steps == nullptr || !parseSteps(*steps, def.steps) || def.steps.empty()) return false;
    out = std::move(def);
    return true;
}

// ---------------------------------------------------------------- pure rules

// A retry (WELCOME HOME at least once) skips the whole intro.
inline bool introShouldRun(const IntroScriptDef& def, const IntroFlags& flags) {
    if (def.empty()) return false;
    const auto it = flags.find(def.skipFlag);
    return it == flags.end() || it->second < 1;
}

// "About 25%" of max HP, at least 1.
inline int introHitDamage(int maxHp, int pct) {
    return std::max(1, static_cast<int>(std::lround(static_cast<double>(maxHp) * pct / 100.0)));
}

// The tutorial can't kill: HP never drops below 1.
inline int introHpAfterHit(int hp, int damage) {
    return std::max(1, hp - damage);
}

// Which encounter the fight runs in, given the locked variant: 1 petals ("floofy"), 2 bass ("floofy_bass").
inline std::string introFightEncounter(int variant) {
    return variant == FLOOFY_FIGHT_BASS ? "floofy_bass" : "floofy";
}

// ---------------------------------------------------------------- the machine

enum class IntroPhase {
    Text,     // a page is on screen, waiting for confirm
    Choice,   // waiting for chooseOption()
    Petals,   // waiting for petalsDodged() / petalsHit()
    Pause,    // timed, driven by tick()
    Finished
};

enum class IntroEffectKind {
    StopMusic,
    Sound,
    ShowPetals,
    Flash,
    FurBarFirst,  // FUR-003: the FUR bar shows up for the first time (trembles)
    Wilt          // petals wilt black, the smile gets too wide
};

struct IntroEffect {
    IntroEffectKind kind = IntroEffectKind::Sound;
    std::string id;
    float seconds = 0.0f;
};

enum class IntroRoundKind { Tutorial, Retry, Forced };

struct IntroPetalRound {
    IntroRoundKind kind = IntroRoundKind::Tutorial;
    std::string pattern;
    int count = 0;
    float speed = 0.0f;
    bool dodgeable = true;
};

struct IntroHitResult {
    int damage = 0;
    int furDelta = 0;
    bool furBarFirst = false;
};

struct IntroView {
    IntroSpeaker speaker = IntroSpeaker::Floofy;
    std::string text;
    int page = 0;
    int pageCount = 0;
};

class BattleIntroScript {
public:
    explicit BattleIntroScript(const IntroScriptDef& def) {
        for (const IntroStep& s : def.steps) m_queue.push_back(s);
    }

    // Starts the run: resets this run's flags, then plays until the first thing the player must answer.
    void start(IntroFlags& flags) {
        flags[INTRO_FLAG_DODGES] = 0;
        pump(flags);
    }

    IntroPhase phase() const { return m_phase; }
    bool finished() const { return m_phase == IntroPhase::Finished; }
    int beat() const { return m_beat; }
    int dodges() const { return m_dodges; }
    int fightVariant() const { return m_variant; }

    // Text phase: the current page. Choice phase: the Floofy-less prompt is empty, options() has the labels.
    IntroView view() const {
        IntroView v;
        v.speaker = m_speaker;
        v.pageCount = static_cast<int>(m_lines.size());
        v.page = m_page;
        if (m_phase == IntroPhase::Text && m_page < v.pageCount) v.text = m_lines[static_cast<std::size_t>(m_page)];
        return v;
    }

    std::vector<std::string> options() const {
        std::vector<std::string> labels;
        if (m_phase == IntroPhase::Choice) {
            for (const IntroChoiceOption& o : m_choice.options) labels.push_back(o.label);
        }
        return labels;
    }

    // Petals phase: what the scene should spawn now.
    IntroPetalRound round() const {
        IntroPetalRound r;
        const IntroPetalsDef& p = m_petals.petals;
        r.count = p.count;
        if (m_dodges <= 0) {
            r.kind = IntroRoundKind::Tutorial;
            r.pattern = p.pattern;
            r.speed = p.speed;
        } else if (m_dodges == 1) {
            r.kind = IntroRoundKind::Retry;
            r.pattern = p.pattern;
            r.speed = p.speed * p.retrySpeedScale;
        } else {
            r.kind = IntroRoundKind::Forced;
            r.pattern = p.forcedPattern;
            r.speed = p.speed * p.fastSpeedScale;
            r.dodgeable = false;
        }
        return r;
    }

    // Effects since the last call (the scene plays sounds, flashes, stops music).
    std::vector<IntroEffect> takeEffects() {
        std::vector<IntroEffect> out;
        out.swap(m_effects);
        return out;
    }

    // Text phase: next page, or the next step when the last page is confirmed.
    void confirm(IntroFlags& flags) {
        if (m_phase != IntroPhase::Text) return;
        if (m_page + 1 < static_cast<int>(m_lines.size())) {
            ++m_page;
            return;
        }
        m_phase = IntroPhase::Pause;  // leaves Text before pumping
        pump(flags);
    }

    // Choice phase: stores the flag, annoys, plays the reply.
    bool chooseOption(int index, IntroFlags& flags) {
        if (m_phase != IntroPhase::Choice) return false;
        if (index < 0 || index >= static_cast<int>(m_choice.options.size())) return false;
        const IntroChoiceOption& opt = m_choice.options[static_cast<std::size_t>(index)];
        if (!m_choice.flag.empty()) flags[m_choice.flag] = opt.value;
        if (opt.annoy > 0) flags[INTRO_FLAG_ANNOYED] += opt.annoy;
        injectFront(opt.reply);
        pump(flags);
        return true;
    }

    // Timed pauses.
    void tick(float dt, IntroFlags& flags) {
        if (m_phase != IntroPhase::Pause || m_pauseLeft <= 0.0f) return;
        m_pauseLeft -= dt;
        if (m_pauseLeft <= 0.0f) {
            m_pauseLeft = 0.0f;
            pump(flags);
        }
    }

    // Petals phase: every petal of the round passed without touching the soul.
    // Returns false for the forced round (it can't be dodged; the scene must make the hit land).
    bool petalsDodged(IntroFlags& flags) {
        if (m_phase != IntroPhase::Petals || !round().dodgeable) return false;
        ++m_dodges;
        flags[INTRO_FLAG_DODGES] = m_dodges;
        if (m_dodges >= 2 && m_petals.petals.annoySecondDodge > 0) {
            flags[INTRO_FLAG_ANNOYED] += m_petals.petals.annoySecondDodge;
        }
        IntroStep next;
        next.kind = IntroStepKind::NextRound;
        m_queue.push_front(std::move(next));  // after the branch text, the next round starts
        injectFront(m_dodges == 1 ? m_petals.petals.onDodgeOnce : m_petals.petals.onDodgeTwice);
        pump(flags);
        return true;
    }

    // Petals phase: a petal touched the soul (any round). Returns the numbers the scene applies:
    // damage (HP is floored at 1 by introHpAfterHit), FUR delta (capped), and whether this is the
    // first time the FUR bar shows (FUR-003 tremble).
    IntroHitResult petalsHit(IntroFlags& flags, int maxHp, int currentFur) {
        IntroHitResult r;
        if (m_phase != IntroPhase::Petals) return r;
        const IntroPetalsDef& p = m_petals.petals;
        r.damage = introHitDamage(maxHp, p.damagePct);
        r.furDelta = battleFurDelta(currentFur, p.fur, p.furCap);
        const auto seen = flags.find(INTRO_FLAG_FUR_SEEN);
        r.furBarFirst = currentFur <= 0 && r.furDelta > 0 && (seen == flags.end() || seen->second == 0);
        if (r.furBarFirst) m_effects.push_back({ IntroEffectKind::FurBarFirst, {}, 0.0f });
        m_effects.push_back({ IntroEffectKind::Wilt, {}, 0.0f });
        injectFront(p.afterHit);
        pump(flags);
        return r;
    }

private:
    void injectFront(const std::vector<IntroStep>& steps) {
        for (auto it = steps.rbegin(); it != steps.rend(); ++it) m_queue.push_front(*it);
    }

    void showText(const IntroStep& s, IntroSpeaker who) {
        m_lines = s.lines;
        m_page = 0;
        m_speaker = who;
        m_phase = IntroPhase::Text;
    }

    // Runs instant steps until something waits for the player.
    void pump(IntroFlags& flags) {
        m_phase = IntroPhase::Pause;
        m_pauseLeft = 0.0f;
        while (true) {
            if (m_queue.empty()) {
                m_phase = IntroPhase::Finished;
                return;
            }
            IntroStep s = std::move(m_queue.front());
            m_queue.pop_front();
            if (s.beat > 0) m_beat = s.beat;
            switch (s.kind) {
            case IntroStepKind::Say:
                showText(s, IntroSpeaker::Floofy);
                return;
            case IntroStepKind::Narrate:
                showText(s, IntroSpeaker::Narrator);
                return;
            case IntroStepKind::Choice:
                m_choice = std::move(s);
                m_phase = IntroPhase::Choice;
                return;
            case IntroStepKind::Petals:
                m_petals = std::move(s);
                m_dodges = 0;
                m_phase = IntroPhase::Petals;
                return;
            case IntroStepKind::NextRound:
                m_phase = IntroPhase::Petals;
                return;
            case IntroStepKind::StopMusic:
                m_effects.push_back({ IntroEffectKind::StopMusic, {}, 0.0f });
                break;
            case IntroStepKind::Sound:
                m_effects.push_back({ IntroEffectKind::Sound, s.id, 0.0f });
                break;
            case IntroStepKind::ShowPetals:
                m_effects.push_back({ IntroEffectKind::ShowPetals, {}, 0.0f });
                break;
            case IntroStepKind::Flash:
                m_effects.push_back({ IntroEffectKind::Flash, s.id, s.seconds });
                m_pauseLeft = s.seconds;
                m_phase = IntroPhase::Pause;
                return;
            case IntroStepKind::Pause:
                m_pauseLeft = s.seconds;
                if (m_pauseLeft > 0.0f) {
                    m_phase = IntroPhase::Pause;
                    return;
                }
                break;
            case IntroStepKind::PickFight: {
                const int annoyed = flags.count(INTRO_FLAG_ANNOYED) != 0U ? flags[INTRO_FLAG_ANNOYED] : 0;
                m_variant = floofyLockFight(flags, annoyed);
                injectFront(m_variant == FLOOFY_FIGHT_BASS ? s.bassFight : s.petalsFight);
                break;
            }
            }
        }
    }

    std::deque<IntroStep> m_queue;
    IntroPhase m_phase = IntroPhase::Pause;
    IntroSpeaker m_speaker = IntroSpeaker::Floofy;
    std::vector<std::string> m_lines;
    int m_page = 0;
    float m_pauseLeft = 0.0f;
    int m_beat = 0;
    int m_dodges = 0;
    int m_variant = 0;
    IntroStep m_choice;
    IntroStep m_petals;
    std::vector<IntroEffect> m_effects;
};

}  // namespace nundertale
