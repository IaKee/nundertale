#include "scenes/ShopScene.hpp"
#include "core/Constants.hpp"
#include "core/GameContext.hpp"
#include "core/SceneManager.hpp"
#include <raylib.h>
#include <algorithm>
#include <sstream>
#include <utility>

namespace nundertale {

namespace {

constexpr const char* MENU_MOVE = "assets/sfx/menu_move.wav";
constexpr const char* MENU_SELECT = "assets/sfx/menu_select.wav";
constexpr const char* MENU_ERROR = "assets/sfx/menu_error.ogg";

} // namespace

ShopScene::ShopScene(std::string shopId)
    : m_shopId(std::move(shopId)) {}

void ShopScene::onEnter(GameContext& context) {
    context.raidPaused = true;
    m_view = View::Main;
    m_returnView = View::Main;
    m_selectedIndex = 0;
    m_message.clear();
    m_exitAfterMessage = false;
    m_valid = loadShopData("data/shops/" + m_shopId + ".json", m_data) && m_data.id == m_shopId;
}

void ShopScene::onExit(GameContext& context) {
    context.raidPaused = false;
}

void ShopScene::beginMessage(const std::string& text, View returnView, bool exitAfter) {
    m_message = text;
    m_returnView = returnView;
    m_exitAfterMessage = exitAfter;
    m_view = View::Message;
    m_selectedIndex = 0;
}

void ShopScene::acknowledgeMessage(GameContext& context) {
    context.audio.playSound(MENU_SELECT, 0.7f);
    if (m_exitAfterMessage) {
        context.scenes.popScene(context);
        return;
    }
    m_view = m_returnView;
    m_selectedIndex = 0;
}

void ShopScene::update(float /*dt*/, GameContext& context) {
    if (!m_valid) {
        context.scenes.popScene(context);
        return;
    }

    if (m_view == View::Message) {
        if (context.input.isConfirmPressed() || context.input.isCancelPressed() ||
            context.input.isEscapePressed()) {
            acknowledgeMessage(context);
        }
        return;
    }

    if (context.input.isCancelPressed() || context.input.isEscapePressed()) {
        context.audio.playSound(MENU_SELECT, 0.7f);
        if (m_view == View::Main) {
            beginMessage(m_data.exitText, View::Main, true);
        } else {
            m_view = View::Main;
            m_selectedIndex = 0;
        }
        return;
    }

    int optionCount = 4;
    if (m_view == View::Buy) optionCount = static_cast<int>(m_data.items.size()) + 1;
    if (m_view == View::Talk) optionCount = static_cast<int>(m_data.talk.size()) + 1;

    if (context.input.isUpPressed()) {
        m_selectedIndex = wrapShopIndex(m_selectedIndex, -1, optionCount);
        context.audio.playSound(MENU_MOVE, 0.5f);
    } else if (context.input.isDownPressed()) {
        m_selectedIndex = wrapShopIndex(m_selectedIndex, 1, optionCount);
        context.audio.playSound(MENU_MOVE, 0.5f);
    }

    if (!context.input.isConfirmPressed()) return;
    context.audio.playSound(MENU_SELECT, 0.7f);

    if (m_view == View::Main) {
        switch (m_selectedIndex) {
            case 0:
                m_view = View::Buy;
                m_selectedIndex = 0;
                break;
            case 1:
                beginMessage(m_data.sellRefusal, View::Main);
                break;
            case 2:
                m_view = View::Talk;
                m_selectedIndex = 0;
                break;
            case 3:
                beginMessage(m_data.exitText, View::Main, true);
                break;
            default:
                break;
        }
        return;
    }

    if (m_view == View::Buy) {
        if (m_selectedIndex == static_cast<int>(m_data.items.size())) {
            m_view = View::Main;
            m_selectedIndex = 0;
            return;
        }
        const ShopPurchaseResult result = tryShopPurchase(
            m_data.items[static_cast<std::size_t>(m_selectedIndex)], context.player.gold,
            context.player.inventory, PlayerData::MAX_INVENTORY_SIZE);
        if (result == ShopPurchaseResult::Success) {
            beginMessage(m_data.buyText, View::Buy);
        } else if (result == ShopPurchaseResult::NotEnoughGold) {
            context.audio.playSound(MENU_ERROR, 0.65f);
            beginMessage(m_data.noGold, View::Buy);
        } else {
            context.audio.playSound(MENU_ERROR, 0.65f);
            beginMessage(m_data.fullText, View::Buy);
        }
        return;
    }

    if (m_selectedIndex == static_cast<int>(m_data.talk.size())) {
        m_view = View::Main;
        m_selectedIndex = 0;
        return;
    }
    const ShopTalkTopic& topic = m_data.talk[static_cast<std::size_t>(m_selectedIndex)];
    std::string combined;
    for (const std::string& line : topic.lines) {
        if (!combined.empty()) combined += '\n';
        combined += line;
    }
    beginMessage(combined, View::Talk);
}

void ShopScene::drawSoul(float x, float y) const {
    DrawRectangle(static_cast<int>(x) + 2, static_cast<int>(y), 4, 3, SOUL_RED);
    DrawRectangle(static_cast<int>(x) + 8, static_cast<int>(y), 4, 3, SOUL_RED);
    DrawRectangle(static_cast<int>(x) + 1, static_cast<int>(y) + 3, 12, 4, SOUL_RED);
    DrawRectangle(static_cast<int>(x) + 2, static_cast<int>(y) + 7, 10, 3, SOUL_RED);
    DrawRectangle(static_cast<int>(x) + 4, static_cast<int>(y) + 10, 6, 2, SOUL_RED);
    DrawRectangle(static_cast<int>(x) + 6, static_cast<int>(y) + 12, 2, 2, SOUL_RED);
}

void ShopScene::drawWrappedText(GameContext& context, const std::string& text, float x, float y,
                                float maxWidth, float fontSize, float lineHeight) const {
    Font font = context.resources.getFont8Bit(static_cast<int>(fontSize));
    std::istringstream paragraphs(text);
    std::string paragraph;
    float drawY = y;
    while (std::getline(paragraphs, paragraph)) {
        std::istringstream words(paragraph);
        std::string word;
        std::string line;
        while (words >> word) {
            const std::string candidate = line.empty() ? word : line + " " + word;
            if (!line.empty() && context.resources.measureTextWidth(font, candidate, fontSize, 1.0f) > maxWidth) {
                context.resources.drawText(font, line, { x, drawY }, fontSize, WHITE, 1.0f);
                drawY += lineHeight;
                line = word;
            } else {
                line = candidate;
            }
        }
        if (!line.empty()) {
            context.resources.drawText(font, line, { x, drawY }, fontSize, WHITE, 1.0f);
        }
        drawY += lineHeight;
    }
}

void ShopScene::render(GameContext& context) {
    ClearBackground(BLACK);
    if (!m_valid) return;

    Font titleFont = context.resources.getFont8Bit(24);
    Font menuFont = context.resources.getFont8Bit(20);
    Font smallFont = context.resources.getFont8Bit(16);

    context.resources.drawCenteredText(titleFont, m_data.name, 16.0f, 24.0f, WHITE, 1.0f);

    DrawLine(70, 196, 570, 196, Color{ 80, 80, 80, 255 });
    DrawLine(92, 58, 60, 90, Color{ 70, 70, 70, 255 });
    DrawLine(548, 58, 580, 90, Color{ 70, 70, 70, 255 });
    Texture2D booth = context.resources.getTexture("assets/props/ruins_booth.png");
    if (booth.id > 0 && booth.width > 0 && booth.height > 0) {
        SetTextureFilter(booth, TEXTURE_FILTER_POINT);
        const float scale = 3.0f;
        const Rectangle source = { 0.0f, 0.0f, static_cast<float>(booth.width), static_cast<float>(booth.height) };
        const Rectangle dest = {
            320.0f - static_cast<float>(booth.width) * scale * 0.5f,
            70.0f,
            static_cast<float>(booth.width) * scale,
            static_cast<float>(booth.height) * scale
        };
        DrawTexturePro(booth, source, dest, { 0.0f, 0.0f }, 0.0f, WHITE);
    }

    DrawRectangle(432, 153, 29, 34, Color{ 55, 55, 65, 255 });
    DrawRectangleLinesEx({ 432.0f, 153.0f, 29.0f, 34.0f }, 3.0f, WHITE);
    DrawRectangle(437, 148, 19, 7, WHITE);
    DrawCircle(440, 178, 3.0f, GOLD);
    DrawCircle(449, 174, 3.0f, GOLD);
    DrawCircle(455, 181, 3.0f, GOLD);

    const Rectangle leftBox = { 16.0f, 244.0f, 420.0f, 220.0f };
    const Rectangle rightBox = { 444.0f, 244.0f, 180.0f, 220.0f };
    DrawRectangleLinesEx(leftBox, 5.0f, WHITE);
    DrawRectangleLinesEx(rightBox, 5.0f, WHITE);

    if (m_view == View::Main) {
        drawWrappedText(context, m_data.greeting, 36.0f, 270.0f, 378.0f, 19.0f, 30.0f);
        const char* options[] = { "Buy", "Sell", "Talk", "Exit" };
        for (int i = 0; i < 4; ++i) {
            const float y = 268.0f + static_cast<float>(i) * 36.0f;
            context.resources.drawText(menuFont, options[i], { 482.0f, y }, 20.0f, WHITE, 1.0f);
        }
        drawSoul(460.0f, 272.0f + static_cast<float>(m_selectedIndex) * 36.0f);
    } else if (m_view == View::Buy) {
        for (std::size_t i = 0; i < m_data.items.size(); ++i) {
            const float y = 270.0f + static_cast<float>(i) * 36.0f;
            const std::string label = std::to_string(m_data.items[i].price) + "G - " + m_data.items[i].item;
            context.resources.drawText(menuFont, label, { 60.0f, y }, 20.0f, WHITE, 1.0f);
        }
        const float exitY = 270.0f + static_cast<float>(m_data.items.size()) * 36.0f;
        context.resources.drawText(menuFont, "Exit", { 60.0f, exitY }, 20.0f, WHITE, 1.0f);
        drawSoul(34.0f, 274.0f + static_cast<float>(m_selectedIndex) * 36.0f);
        context.resources.drawText(titleFont, "BUY", { 496.0f, 276.0f }, 24.0f, WHITE, 1.0f);
        drawWrappedText(context, "* Put your gold in the jar.", 462.0f, 326.0f, 144.0f, 16.0f, 24.0f);
    } else if (m_view == View::Talk) {
        for (std::size_t i = 0; i < m_data.talk.size(); ++i) {
            const float y = 270.0f + static_cast<float>(i) * 36.0f;
            context.resources.drawText(menuFont, m_data.talk[i].label, { 60.0f, y }, 20.0f, WHITE, 1.0f);
        }
        const float exitY = 270.0f + static_cast<float>(m_data.talk.size()) * 36.0f;
        context.resources.drawText(menuFont, "Exit", { 60.0f, exitY }, 20.0f, WHITE, 1.0f);
        drawSoul(34.0f, 274.0f + static_cast<float>(m_selectedIndex) * 36.0f);
        context.resources.drawText(titleFont, "TALK", { 486.0f, 276.0f }, 24.0f, WHITE, 1.0f);
        drawWrappedText(context, "* The jar is a good listener.", 462.0f, 326.0f, 144.0f, 16.0f, 24.0f);
    } else {
        drawWrappedText(context, m_message, 36.0f, 270.0f, 378.0f, 19.0f, 30.0f);
        context.resources.drawText(smallFont, "Z - Continue", { 474.0f, 344.0f }, 16.0f, COLOR_UNDERTALE_GRAY, 1.0f);
    }

    context.resources.drawText(smallFont, std::to_string(context.player.gold) + " G",
                               { 466.0f, 414.0f }, 16.0f, WHITE, 1.0f);
    context.resources.drawText(smallFont,
                               "Space - " + std::to_string(context.player.inventory.size()) + "/" +
                                   std::to_string(PlayerData::MAX_INVENTORY_SIZE),
                               { 466.0f, 438.0f }, 16.0f, WHITE, 1.0f);
}

} // namespace nundertale
