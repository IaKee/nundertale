#pragma once

#include <algorithm>

namespace nundertale {

struct CameraTopLeft {
    float x = 0.0f;
    float y = 0.0f;
};

class Camera2DFollow {
public:
    static CameraTopLeft calculate(float focusX,
                                   float focusY,
                                   float roomWidth,
                                   float roomHeight,
                                   float viewportWidth = 640.0f,
                                   float viewportHeight = 480.0f) {
        return {
            axisTopLeft(focusX, roomWidth, viewportWidth),
            axisTopLeft(focusY, roomHeight, viewportHeight)
        };
    }

private:
    static float axisTopLeft(float focus, float roomSize, float viewportSize) {
        if (roomSize <= viewportSize) {
            return (roomSize - viewportSize) * 0.5f;
        }
        return std::clamp(focus - viewportSize * 0.5f, 0.0f, roomSize - viewportSize);
    }
};

} // namespace nundertale
