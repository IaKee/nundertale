#pragma once

#include <raylib.h>
#include <vector>
#include <cmath>
#include <cstdint>
#include <algorithm>

namespace nundertale {

using RaylibRandomByteFn = uint8_t(*)();
inline RaylibRandomByteFn s_raylibRandomByteFn = nullptr;

inline void setMosaicRaylibRandomByteFn(RaylibRandomByteFn fn) {
    s_raylibRandomByteFn = fn;
}

class Mosaic {
public:
    explicit Mosaic(Rectangle area, int blockSize = 8, int reshuffleFrames = 4, unsigned seed = 0)
        : m_area(area),
          m_blockSize((blockSize > 0) ? blockSize : 8),
          m_reshuffleFrames(reshuffleFrames),
          m_seed(seed),
          m_rngState((seed != 0) ? static_cast<uint32_t>(seed) : 123456789u) {
        initGrid();
    }

    void setArea(Rectangle area) {
        int oldCols = columns();
        int oldRows = rows();
        std::vector<Color> oldColors = m_colors;

        m_area = area;
        int newCols = columns();
        int newRows = rows();

        m_colors.assign(static_cast<size_t>(newCols * newRows), Color{ 0, 0, 0, 255 });
        for (int r = 0; r < newRows; ++r) {
            for (int c = 0; c < newCols; ++c) {
                if (r < oldRows && c < oldCols) {
                    m_colors[static_cast<size_t>(r * newCols + c)] = oldColors[static_cast<size_t>(r * oldCols + c)];
                } else {
                    m_colors[static_cast<size_t>(r * newCols + c)] = generateRandomColor();
                }
            }
        }
    }

    void update(float dt) {
        if (m_reshuffleFrames <= 0) return;

        m_frameAccumulator += dt * 30.0f;
        while (m_frameAccumulator >= 1.0f - 1e-4f) {
            m_frameAccumulator -= 1.0f;
            if (m_frameAccumulator < 0.0f) {
                m_frameAccumulator = 0.0f;
            }
            m_logicFrames++;
            if (m_logicFrames >= m_reshuffleFrames) {
                m_logicFrames = 0;
                reshuffle();
            }
        }
    }

    void render() const;

    int columns() const {
        if (m_area.width <= 0.0f || m_blockSize <= 0) return 0;
        return static_cast<int>(std::ceil(m_area.width / static_cast<float>(m_blockSize)));
    }

    int rows() const {
        if (m_area.height <= 0.0f || m_blockSize <= 0) return 0;
        return static_cast<int>(std::ceil(m_area.height / static_cast<float>(m_blockSize)));
    }

    Color blockColor(int col, int row) const {
        int cols = columns();
        int rws = rows();
        if (col < 0 || col >= cols || row < 0 || row >= rws) {
            return Color{ 0, 0, 0, 0 };
        }
        return m_colors[static_cast<size_t>(row * cols + col)];
    }

    Rectangle blockRect(int col, int row) const {
        int cols = columns();
        int rws = rows();
        if (col < 0 || col >= cols || row < 0 || row >= rws) {
            return Rectangle{ 0.0f, 0.0f, 0.0f, 0.0f };
        }

        float bx = m_area.x + static_cast<float>(col * m_blockSize);
        float by = m_area.y + static_cast<float>(row * m_blockSize);
        float bw = static_cast<float>(m_blockSize);
        float bh = static_cast<float>(m_blockSize);

        float areaRight = m_area.x + m_area.width;
        float areaBottom = m_area.y + m_area.height;

        if (bx + bw > areaRight) {
            bw = areaRight - bx;
        }
        if (bw < 0.0f) bw = 0.0f;

        if (by + bh > areaBottom) {
            bh = areaBottom - by;
        }
        if (bh < 0.0f) bh = 0.0f;

        return Rectangle{ bx, by, bw, bh };
    }

    int shuffleCount() const {
        return m_shuffleCount;
    }

private:
    Rectangle m_area{ 0.0f, 0.0f, 0.0f, 0.0f };
    int m_blockSize = 8;
    int m_reshuffleFrames = 4;
    unsigned m_seed = 0;
    uint32_t m_rngState = 123456789u;

    std::vector<Color> m_colors;
    float m_frameAccumulator = 0.0f;
    int m_logicFrames = 0;
    int m_shuffleCount = 0;

    uint8_t nextRandomByte() {
        if (m_seed == 0) {
            if (s_raylibRandomByteFn) {
                return s_raylibRandomByteFn();
            }
            m_rngState = m_rngState * 1664525u + 1013904223u;
            return static_cast<uint8_t>((m_rngState >> 16) & 0xFF);
        }
        m_rngState = m_rngState * 1664525u + 1013904223u;
        return static_cast<uint8_t>((m_rngState >> 16) & 0xFF);
    }

    Color generateRandomColor() {
        uint8_t r = nextRandomByte();
        uint8_t g = nextRandomByte();
        uint8_t b = nextRandomByte();
        return Color{ r, g, b, 255 };
    }

    void initGrid() {
        int cols = columns();
        int rws = rows();
        size_t total = static_cast<size_t>(cols * rws);
        m_colors.resize(total);
        for (size_t i = 0; i < total; ++i) {
            m_colors[i] = generateRandomColor();
        }
    }

    void reshuffle() {
        int cols = columns();
        int rws = rows();
        size_t total = static_cast<size_t>(cols * rws);
        m_colors.resize(total);
        for (size_t i = 0; i < total; ++i) {
            m_colors[i] = generateRandomColor();
        }
        m_shuffleCount++;
    }
};

} // namespace nundertale
