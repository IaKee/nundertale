#pragma once

#include "core/IScene.hpp"
#include <vector>
#include <string>

namespace nundertale {

class TitleScene : public IScene {
public:
    TitleScene() = default;
    ~TitleScene() override = default;

    void onEnter(GameContext& context) override;
    void update(float dt, GameContext& context) override;
    void render(GameContext& context) override;
    void onExit(GameContext& context) override;

private:
    std::vector<std::string> m_options{ "Novo Jogo", "Instrucoes", "Creditos", "Sair" };
    int m_selectedIndex = 0;
    float m_animTimer = 0.0f;
};

} // namespace nundertale
