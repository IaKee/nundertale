#pragma once

#include "Constants.hpp"
#include "GameContext.hpp"
#include "SceneManager.hpp"
#include "services/IAudioService.hpp"
#include "services/IInputService.hpp"
#include "services/ResourceService.hpp"
#include "services/SaveService.hpp"
#include <algorithm>
#include <cmath>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace nundertale {

struct VirtualCanvasGeometry {
    float scale = 1.0f;
    float x = 0.0f;
    float y = 0.0f;
    float width = static_cast<float>(VIRTUAL_WIDTH);
    float height = static_cast<float>(VIRTUAL_HEIGHT);
};

inline VirtualCanvasGeometry calculateVirtualCanvasGeometry(
    int windowWidth, int windowHeight, bool integerScale) {
    float scale = std::min(
        static_cast<float>(windowWidth) / static_cast<float>(VIRTUAL_WIDTH),
        static_cast<float>(windowHeight) / static_cast<float>(VIRTUAL_HEIGHT));
    if (integerScale) {
        scale = std::max(1.0f, std::floor(scale));
    }

    const float width = static_cast<float>(VIRTUAL_WIDTH) * scale;
    const float height = static_cast<float>(VIRTUAL_HEIGHT) * scale;
    float x = (static_cast<float>(windowWidth) - width) * 0.5f;
    float y = (static_cast<float>(windowHeight) - height) * 0.5f;
    if (integerScale) {
        // Keep the texture origin on physical pixels. Odd-sized letterboxes put
        // the unavoidable extra pixel on the right/bottom edge.
        x = std::floor(x);
        y = std::floor(y);
    }

    return { scale, x, y, width, height };
}

struct HoldToQuitState {
    static constexpr float HOLD_SECONDS = 1.5f;
    static constexpr int REQUIRED_TICKS = TARGET_FPS * 3 / 2;

    bool update(bool held) {
        if (!held) {
            reset();
            return false;
        }
        if (m_heldTicks < REQUIRED_TICKS) ++m_heldTicks;
        return complete();
    }

    void reset() { m_heldTicks = 0; }
    bool active() const { return m_heldTicks > 0; }
    bool complete() const { return m_heldTicks >= REQUIRED_TICKS; }
    int heldTicks() const { return m_heldTicks; }
    float progress() const {
        return static_cast<float>(m_heldTicks) / static_cast<float>(REQUIRED_TICKS);
    }

private:
    int m_heldTicks = 0;
};

class Game {
public:
    Game(int argc, char** argv);
    ~Game();

    void run();

private:
    void initWindow();
    void renderLetterboxed();

    std::unique_ptr<IAudioService> m_audioService;
    std::unique_ptr<ResourceService> m_resourceService;
    std::unique_ptr<IInputService> m_inputService;
    std::unique_ptr<SaveService> m_saveService;
    std::unique_ptr<SceneManager> m_sceneManager;

    RenderTexture2D m_virtualTarget{};
    std::unique_ptr<GameContext> m_context;

#ifdef NUNDERTALE_DEBUG
    bool m_debugLaunch = false;
    std::string m_debugRoom;
    std::string m_debugSpawn;
    std::string m_debugBattle;
    std::vector<std::pair<std::string, int>> m_debugFlags;
    std::string m_debugShotPath;
    int m_debugShotFrames = 20;
    int m_debugTicks = 0;
    std::string m_debugRecordDirectory;
    std::string m_debugInputPath;
    double m_debugRecordSeconds = 30.0;
    int m_debugRecordTickLimit = 900;
#endif
};

} // namespace nundertale
