#include "ui/Mosaic.hpp"

namespace nundertale {

static uint8_t defaultRaylibRandomByte() {
    return static_cast<uint8_t>(GetRandomValue(0, 255));
}

struct MosaicRaylibInit {
    MosaicRaylibInit() {
        setMosaicRaylibRandomByteFn(defaultRaylibRandomByte);
    }
};

static MosaicRaylibInit s_mosaicRaylibInit;

void Mosaic::render() const {
    int cols = columns();
    int rws = rows();
    for (int r = 0; r < rws; ++r) {
        for (int c = 0; c < cols; ++c) {
            Rectangle rect = blockRect(c, r);
            if (rect.width > 0.0f && rect.height > 0.0f) {
                DrawRectangleRec(rect, blockColor(c, r));
            }
        }
    }
}

} // namespace nundertale
