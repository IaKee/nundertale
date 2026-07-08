#pragma once

#include <cstddef>

namespace nundertale {

// Maps a 1-based ACT use count to an available per-use entry.
// Invalid/early counts select the first entry; uses beyond the data select the last.
inline std::size_t actUseIndex(int useCount, std::size_t entryCount) {
    if (entryCount == 0U || useCount <= 1) return 0U;

    const std::size_t requested = static_cast<std::size_t>(useCount - 1);
    return requested < entryCount ? requested : entryCount - 1U;
}

} // namespace nundertale
