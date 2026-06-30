#pragma once

#include "Constants.hpp"
#include "GameContext.hpp"
#include "SceneManager.hpp"
#include "services/IAudioService.hpp"
#include "services/IInputService.hpp"
#include "services/ResourceService.hpp"
#include "services/SaveService.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace nundertale {

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
