#pragma once

#include "core/IScene.hpp"
#include "scenes/ShopModel.hpp"
#include <string>
#include <vector>

namespace nundertale {

class ShopScene final : public IScene {
public:
    explicit ShopScene(std::string shopId);
    ~ShopScene() override = default;

    void onEnter(GameContext& context) override;
    void update(float dt, GameContext& context) override;
    void render(GameContext& context) override;
    void onExit(GameContext& context) override;

private:
    enum class View {
        Main,
        Buy,
        Talk,
        Message
    };

    void beginMessage(const std::string& text, View returnView, bool exitAfter = false);
    void acknowledgeMessage(GameContext& context);
    void drawSoul(float x, float y) const;
    void drawWrappedText(GameContext& context, const std::string& text, float x, float y,
                         float maxWidth, float fontSize, float lineHeight) const;

    std::string m_shopId;
    ShopData m_data;
    View m_view = View::Main;
    View m_returnView = View::Main;
    int m_selectedIndex = 0;
    std::string m_message;
    bool m_exitAfterMessage = false;
    bool m_valid = false;
};

} // namespace nundertale
