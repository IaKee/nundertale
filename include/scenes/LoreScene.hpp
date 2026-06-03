#pragma once

#include "core/IScene.hpp"
#include <string>
#include <vector>

namespace nundertale {

struct LoreCard {
    std::string imagePath;
    std::vector<std::string> lines;
    // ART-041: 200x110 panel drawn at 2x; the legacy 1x BMP above is the fallback
    std::string panelPath;
};

class LoreScene : public IScene {
public:
    LoreScene() = default;
    ~LoreScene() override = default;

    void onEnter(GameContext& context) override;
    void update(float dt, GameContext& context) override;
    void render(GameContext& context) override;
    void onExit(GameContext& context) override;

private:
    std::vector<LoreCard> m_cards;
    size_t m_currentCard = 0;
    size_t m_currentLine = 0;
    size_t m_charIndex = 0;

    float m_charTimer = 0.0f;
    float m_cardTimer = 0.0f;
    float m_cardAlpha = 0.0f;
    bool m_cardFadingIn = true;
    bool m_cardTextDone = false;

    // Card 10 animation sequence
    bool m_isPlayingCard10Anim = false;
    int m_card10Frame = 0;
    float m_card10Timer = 0.0f;
    static constexpr int CARD10_TOTAL_FRAMES = 271;

    // True when every card has its new panel: the legacy lore10 frames would clash with it,
    // so the intro cuts to the title after card 10 instead
    bool m_usePanels = false;

    // Card 10 tall scroll (T2)
    bool m_hasTallCard10 = false;
    float m_tallScrollTimer = 0.0f;
};

} // namespace nundertale
