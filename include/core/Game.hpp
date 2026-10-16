#pragma once

#include "Constants.hpp"
#include "GameContext.hpp"
#include "SceneManager.hpp"
#include "services/AudioService.hpp"
#include "services/ResourceService.hpp"
#include "services/InputService.hpp"
#include "services/SaveService.hpp"
#include <memory>

namespace nundertale {

class Game {
public:
    Game();
    ~Game();

    void run();

private:
    void initWindow();
    void renderLetterboxed();

    std::unique_ptr<AudioService> m_audioService;
    std::unique_ptr<ResourceService> m_resourceService;
    std::unique_ptr<InputService> m_inputService;
    std::unique_ptr<SaveService> m_saveService;
    std::unique_ptr<SceneManager> m_sceneManager;

    RenderTexture2D m_virtualTarget{};
    std::unique_ptr<GameContext> m_context;
};

} // namespace nundertale
