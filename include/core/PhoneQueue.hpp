#pragma once

#include <algorithm>
#include <cstddef>
#include <deque>
#include <optional>
#include <string>
#include <utility>

namespace nundertale {

struct PhonePlaybackState {
    bool cutsceneActive = false;
    bool menuOpen = false;
    bool battleActive = false;

    bool playerFree() const {
        return !cutsceneActive && !menuOpen && !battleActive;
    }
};

// MNU-006: FIFO for automatic overworld messages and forced calls. Message keys are
// deliberately opaque: the overworld may resolve them to a cutscene or to generated pages.
class PhoneQueue {
public:
    bool enqueue(std::string messageKey) {
        if (messageKey.empty() || contains(messageKey)) return false;
        m_messages.push_back(std::move(messageKey));
        return true;
    }

    std::optional<std::string> takeNext(const PhonePlaybackState& state) {
        if (!state.playerFree() || m_messages.empty()) return std::nullopt;
        std::string next = std::move(m_messages.front());
        m_messages.pop_front();
        return next;
    }

    bool empty() const { return m_messages.empty(); }
    std::size_t size() const { return m_messages.size(); }

    bool contains(const std::string& messageKey) const {
        return std::find(m_messages.begin(), m_messages.end(), messageKey) != m_messages.end();
    }

    void clear() { m_messages.clear(); }

private:
    std::deque<std::string> m_messages;
};

} // namespace nundertale
