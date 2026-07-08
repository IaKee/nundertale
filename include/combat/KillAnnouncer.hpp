#pragma once

#include "core/GameContext.hpp"
#include "core/Killstreaks.hpp"
#include "core/RandomEncounters.hpp"
#include "core/ToxicTrack.hpp"

#include <cstddef>
#include <deque>
#include <string>
#include <string_view>

namespace nundertale {

enum class KillAnnouncementKind {
    None = 0,
    FirstBlood,
    DoubleKill,
    TripleKill,
    QuadraKill,
    PentaKill,
    Ace
};

struct KillAnnouncement {
    KillAnnouncementKind kind = KillAnnouncementKind::None;
    std::string bannerText;
    std::string subText;
    std::string voiceLine;
    float duration = 0.0f;
};

inline constexpr float DEFAULT_ANNOUNCER_BANNER_DURATION = 3.0f;
inline constexpr float ACE_ANNOUNCER_BANNER_DURATION = 4.0f;

inline constexpr std::string_view TEXT_FIRST_BLOOD = "FIRST BLOOD";
inline constexpr std::string_view TEXT_DOUBLE_KILL = "DOUBLE KILL";
inline constexpr std::string_view TEXT_TRIPLE_KILL = "TRIPLE KILL";
inline constexpr std::string_view TEXT_QUADRA_KILL = "QUADRA KILL";
inline constexpr std::string_view TEXT_PENTA_KILL = "PENTA KILL";
inline constexpr std::string_view TEXT_ACE = "ACE";

inline KillAnnouncement makeAnnouncement(KillAnnouncementKind kind, std::string_view bannerText,
                                         std::string_view voiceLine, float duration,
                                         std::string_view subText = "") {
    KillAnnouncement a;
    a.kind = kind;
    a.bannerText = std::string(bannerText);
    a.voiceLine = std::string(voiceLine);
    a.duration = duration;
    a.subText = std::string(subText);
    return a;
}

inline KillAnnouncement makeFirstBloodAnnouncement(float duration = DEFAULT_ANNOUNCER_BANNER_DURATION) {
    return makeAnnouncement(KillAnnouncementKind::FirstBlood, TEXT_FIRST_BLOOD, TEXT_FIRST_BLOOD, duration);
}

inline KillAnnouncement makeDoubleKillAnnouncement(float duration = DEFAULT_ANNOUNCER_BANNER_DURATION) {
    return makeAnnouncement(KillAnnouncementKind::DoubleKill, TEXT_DOUBLE_KILL, TEXT_DOUBLE_KILL, duration);
}

inline KillAnnouncement makeTripleKillAnnouncement(float duration = DEFAULT_ANNOUNCER_BANNER_DURATION) {
    return makeAnnouncement(KillAnnouncementKind::TripleKill, TEXT_TRIPLE_KILL, TEXT_TRIPLE_KILL, duration);
}

inline KillAnnouncement makeQuadraKillAnnouncement(float duration = DEFAULT_ANNOUNCER_BANNER_DURATION) {
    return makeAnnouncement(KillAnnouncementKind::QuadraKill, TEXT_QUADRA_KILL, TEXT_QUADRA_KILL, duration);
}

inline KillAnnouncement makePentaKillAnnouncement(float duration = DEFAULT_ANNOUNCER_BANNER_DURATION) {
    return makeAnnouncement(KillAnnouncementKind::PentaKill, TEXT_PENTA_KILL, TEXT_PENTA_KILL, duration);
}

inline KillAnnouncement makeAceAnnouncement(float duration = ACE_ANNOUNCER_BANNER_DURATION) {
    return makeAnnouncement(KillAnnouncementKind::Ace, TEXT_ACE, TEXT_ACE, duration, AREA_EXHAUSTED_NOTICE);
}

inline KillAnnouncementKind streakToAnnouncementKind(int streak) {
    switch (streak) {
        case 2: return KillAnnouncementKind::DoubleKill;
        case 3: return KillAnnouncementKind::TripleKill;
        case 4: return KillAnnouncementKind::QuadraKill;
        case 5: return KillAnnouncementKind::PentaKill;
        default: return KillAnnouncementKind::None;
    }
}

inline std::string_view announcementKindBannerText(KillAnnouncementKind kind) {
    switch (kind) {
        case KillAnnouncementKind::FirstBlood: return TEXT_FIRST_BLOOD;
        case KillAnnouncementKind::DoubleKill: return TEXT_DOUBLE_KILL;
        case KillAnnouncementKind::TripleKill: return TEXT_TRIPLE_KILL;
        case KillAnnouncementKind::QuadraKill: return TEXT_QUADRA_KILL;
        case KillAnnouncementKind::PentaKill:  return TEXT_PENTA_KILL;
        case KillAnnouncementKind::Ace:        return TEXT_ACE;
        default: return "";
    }
}

class KillAnnouncer {
public:
    explicit KillAnnouncer(float defaultBannerDuration = DEFAULT_ANNOUNCER_BANNER_DURATION,
                           float aceBannerDuration = ACE_ANNOUNCER_BANNER_DURATION)
        : m_defaultBannerDuration(defaultBannerDuration)
        , m_aceBannerDuration(aceBannerDuration) {}

    // Processes a kill event on the toxic track.
    // totalKills is the total toxic kills (or toxicKills(state)) after this kill.
    // streak is the multikill streak (toxicStreak(state)) after this kill.
    // If area is provided and areaKills reaches toxicEncounterCap(area) for the first time,
    // an ACE announcement is also enqueued.
    void onKill(int totalKills, int streak, std::string_view area = "", int areaKills = 0) {
        if (totalKills == 1) {
            enqueue(makeFirstBloodAnnouncement(m_defaultBannerDuration));
        } else {
            const KillAnnouncementKind kind = streakToAnnouncementKind(streak);
            if (kind != KillAnnouncementKind::None) {
                enqueue(makeAnnouncement(kind, announcementKindBannerText(kind),
                                         announcementKindBannerText(kind), m_defaultBannerDuration));
            }
        }

        if (!area.empty() && areaKills > 0) {
            checkAreaExhausted(area, areaKills);
        }
    }

    // Convenience method passing GameState and current enemyId
    bool onKill(GameState& state, std::string_view enemyId, std::string_view area = "") {
        const bool counted = toxicTrackOnKill(state, enemyId);
        if (!counted) return false;

        killstreaksOnKill(state);

        int areaKills = 0;
        if (!area.empty()) {
            const std::string areaKillsFlag = "kills." + std::string(area);
            state.incFlag(areaKillsFlag);
            areaKills = state.getFlag(areaKillsFlag, 0);
        }

        const int totalKills = toxicKills(state);
        const int streak = toxicStreak(state);

        if (totalKills == 1) {
            enqueue(makeFirstBloodAnnouncement(m_defaultBannerDuration));
        } else {
            const KillAnnouncementKind kind = streakToAnnouncementKind(streak);
            if (kind != KillAnnouncementKind::None) {
                enqueue(makeAnnouncement(kind, announcementKindBannerText(kind),
                                         announcementKindBannerText(kind), m_defaultBannerDuration));
            }
        }

        if (!area.empty()) {
            const std::string announcedFlag = areaExhaustedAnnouncedFlag(area);
            if (areaExhaustedNoticeDue(area, areaKills, state.getFlag(announcedFlag, 0))) {
                state.setFlag(announcedFlag, 1);
                enqueue(makeAceAnnouncement(m_aceBannerDuration));
            }
        }

        return true;
    }

    // Checks and triggers an ACE announcement if area is newly exhausted
    bool checkAreaExhausted(std::string_view area, int areaKills) {
        if (area.empty()) return false;
        if (areaExhausted(area, areaKills) && !m_areaExhaustedAnnounced(area)) {
            m_markAreaExhaustedAnnounced(area);
            enqueue(makeAceAnnouncement(m_aceBannerDuration));
            return true;
        }
        return false;
    }

    void onSave() {
        // Streak resets on save; any banner currently active or enqueued continues its timing.
    }

    void onSave(GameState& state) {
        toxicOnSave(state);
    }

    void enqueue(const KillAnnouncement& announcement) {
        if (!hasActiveBanner()) {
            m_current = announcement;
            m_timeRemaining = announcement.duration;
        } else {
            m_queue.push_back(announcement);
        }
    }

    void update(float dt) {
        if (dt <= 0.0f) return;

        if (hasActiveBanner()) {
            m_timeRemaining -= dt;
            if (m_timeRemaining <= 0.0f) {
                popNextBanner();
            }
        }
    }

    bool hasActiveBanner() const {
        return m_current.kind != KillAnnouncementKind::None;
    }

    const KillAnnouncement& currentBanner() const {
        return m_current;
    }

    float currentRemainingTime() const {
        return m_timeRemaining;
    }

    std::size_t pendingCount() const {
        return m_queue.size();
    }

    bool empty() const {
        return !hasActiveBanner() && m_queue.empty();
    }

    void clear() {
        m_current = KillAnnouncement{};
        m_timeRemaining = 0.0f;
        m_queue.clear();
        m_announcedAreas.clear();
    }

private:
    void popNextBanner() {
        if (!m_queue.empty()) {
            m_current = m_queue.front();
            m_queue.pop_front();
            m_timeRemaining = m_current.duration;
        } else {
            m_current = KillAnnouncement{};
            m_timeRemaining = 0.0f;
        }
    }

    bool m_areaExhaustedAnnounced(std::string_view area) const {
        for (const auto& a : m_announcedAreas) {
            if (a == area) return true;
        }
        return false;
    }

    void m_markAreaExhaustedAnnounced(std::string_view area) {
        m_announcedAreas.emplace_back(area);
    }

    float m_defaultBannerDuration = DEFAULT_ANNOUNCER_BANNER_DURATION;
    float m_aceBannerDuration = ACE_ANNOUNCER_BANNER_DURATION;

    KillAnnouncement m_current;
    float m_timeRemaining = 0.0f;
    std::deque<KillAnnouncement> m_queue;
    std::vector<std::string> m_announcedAreas;
};

} // namespace nundertale
