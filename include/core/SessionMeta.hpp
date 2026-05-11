#pragma once

namespace nundertale {

struct SessionMeta {
    int killsSinceSave = 0;
    bool reloadedAfterFurryOver = false;
    bool reloadedAfterKill = false;
};

inline void sessionOnKill(SessionMeta& meta) {
    ++meta.killsSinceSave;
}

inline void sessionOnSave(SessionMeta& meta) {
    meta.killsSinceSave = 0;
}

inline void sessionOnLoad(SessionMeta& meta, bool afterFurryOver) {
    meta.reloadedAfterKill = meta.killsSinceSave > 0;
    meta.reloadedAfterFurryOver = afterFurryOver;
    meta.killsSinceSave = 0;
}

} // namespace nundertale
