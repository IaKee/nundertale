#include "scenes/SettingsScene.hpp"
#include "scenes/SettingsModel.hpp"
#include "core/GameContext.hpp"
#include "core/Constants.hpp"
#include "core/SceneManager.hpp"
#include <raylib.h>
#include <string>
#include <cmath>

namespace nundertale {

void SettingsScene::onEnter(GameContext& context) {
    context.raidPaused = true;
    m_initialConfig = context.save.getConfig();
    m_config = m_initialConfig;
    m_selectedIndex = 0;
}

bool SettingsScene::isConfigChanged() const {
    return std::fabs(m_config.musicVolume - m_initialConfig.musicVolume) > 1e-4f ||
           std::fabs(m_config.masterVolume - m_initialConfig.masterVolume) > 1e-4f ||
           m_config.fullscreen != m_initialConfig.fullscreen ||
           m_config.integerScale != m_initialConfig.integerScale ||
           m_config.spareDing != m_initialConfig.spareDing;
}

void SettingsScene::onExit(GameContext& context) {
    context.raidPaused = false;
    if (isConfigChanged()) {
        context.save.saveConfig(m_config);
        m_initialConfig = m_config;
    }
}

void SettingsScene::update(float /*dt*/, GameContext& context) {
    // Keep fullscreen flag synchronized if F4 was pressed (handled globally in Game.cpp)
    if (IsKeyPressed(KEY_F4)) {
        m_config.fullscreen = IsWindowFullscreen();
    }

    if (context.input.isCancelPressed() || context.input.isEscapePressed()) {
        context.audio.playSound("assets/sfx/menu_select.wav", 0.7f);
        context.scenes.popScene(context);
        return;
    }

    if (context.input.isUpPressed()) {
        m_selectedIndex = wrapIndex(m_selectedIndex, -1, 6);
        context.audio.playSound("assets/sfx/menu_move.wav", 0.5f);
    } else if (context.input.isDownPressed()) {
        m_selectedIndex = wrapIndex(m_selectedIndex, +1, 6);
        context.audio.playSound("assets/sfx/menu_move.wav", 0.5f);
    }

    switch (m_selectedIndex) {
        case 0: // EXIT
            if (context.input.isConfirmPressed()) {
                context.audio.playSound("assets/sfx/menu_select.wav", 0.7f);
                context.scenes.popScene(context);
                return;
            }
            break;

        case 1: // MUSIC VOLUME
            if (context.input.isLeftPressed()) {
                float newVol = stepVolume(m_config.musicVolume, -1);
                if (std::fabs(newVol - m_config.musicVolume) > 1e-4f) {
                    m_config.musicVolume = newVol;
                    context.audio.setMusicVolume(m_config.musicVolume);
                    context.audio.playSound("assets/sfx/menu_move.wav", 0.5f);
                }
            } else if (context.input.isRightPressed()) {
                float newVol = stepVolume(m_config.musicVolume, +1);
                if (std::fabs(newVol - m_config.musicVolume) > 1e-4f) {
                    m_config.musicVolume = newVol;
                    context.audio.setMusicVolume(m_config.musicVolume);
                    context.audio.playSound("assets/sfx/menu_move.wav", 0.5f);
                }
            }
            break;

        case 2: // MASTER VOLUME
            if (context.input.isLeftPressed()) {
                float newVol = stepVolume(m_config.masterVolume, -1);
                if (std::fabs(newVol - m_config.masterVolume) > 1e-4f) {
                    m_config.masterVolume = newVol;
                    context.audio.setMasterVolume(m_config.masterVolume);
                    context.audio.playSound("assets/sfx/menu_move.wav", 0.5f);
                }
            } else if (context.input.isRightPressed()) {
                float newVol = stepVolume(m_config.masterVolume, +1);
                if (std::fabs(newVol - m_config.masterVolume) > 1e-4f) {
                    m_config.masterVolume = newVol;
                    context.audio.setMasterVolume(m_config.masterVolume);
                    context.audio.playSound("assets/sfx/menu_move.wav", 0.5f);
                }
            }
            break;

        case 3: // FULLSCREEN
            if (context.input.isConfirmPressed() || context.input.isLeftPressed() || context.input.isRightPressed()) {
                ToggleFullscreen();
                m_config.fullscreen = !m_config.fullscreen;
                context.audio.playSound("assets/sfx/menu_move.wav", 0.5f);
            }
            break;

        case 4: // INTEGER SCALE
            if (context.input.isConfirmPressed() || context.input.isLeftPressed() || context.input.isRightPressed()) {
                m_config.integerScale = !m_config.integerScale;
                context.audio.playSound("assets/sfx/menu_move.wav", 0.5f);
            }
            break;

        case 5: // SPARE DING
            if (context.input.isConfirmPressed() || context.input.isLeftPressed() || context.input.isRightPressed()) {
                m_config.spareDing = !m_config.spareDing;
                context.audio.playSound("assets/sfx/menu_move.wav", 0.5f);
            }
            break;

        default:
            break;
    }
}

void SettingsScene::render(GameContext& context) {
    ClearBackground(BLACK);

    Font fontTitle = context.resources.getFont8Bit(48);
    Font fontMenu = context.resources.getFont8Bit(24);
    Font fontControls = context.resources.getFont8Bit(16);

    context.resources.drawCenteredText(fontTitle, "SETTINGS", 40.0f, 48.0f, WHITE, 2.0f);

    struct OptionItem {
        std::string label;
        std::string value;
    };

    OptionItem options[6] = {
        { "EXIT", "" },
        { "MUSIC VOLUME", "< " + std::to_string(volumePercent(m_config.musicVolume)) + "% >" },
        { "MASTER VOLUME", "< " + std::to_string(volumePercent(m_config.masterVolume)) + "% >" },
        { "FULLSCREEN", m_config.fullscreen ? "ON" : "OFF" },
        { "INTEGER SCALE", m_config.integerScale ? "ON" : "OFF" },
        { "SPARE DING", m_config.spareDing ? "ON" : "OFF" }
    };

    const float startY = 110.0f;
    const float stepY = 36.0f;
    const float labelX = 80.0f;
    const float valueRightX = 480.0f;

    for (int i = 0; i < 6; ++i) {
        float y = startY + static_cast<float>(i) * stepY;
        Color color = (m_selectedIndex == i) ? UI_YELLOW : WHITE;

        context.resources.drawText(fontMenu, options[i].label, { labelX, y }, 24.0f, color, 1.0f);

        if (!options[i].value.empty()) {
            float valWidth = context.resources.measureTextWidth(fontMenu, options[i].value, 24.0f, 1.0f);
            context.resources.drawText(fontMenu, options[i].value, { valueRightX - valWidth, y }, 24.0f, color, 1.0f);
        }
    }

    context.resources.drawText(fontMenu, "CONTROLS", { labelX, 340.0f }, 20.0f, COLOR_UNDERTALE_GRAY, 1.0f);
    context.resources.drawText(fontControls, "[Z] Confirm  [X] Cancel  [C] Menu  [F4] Fullscreen  [Hold ESC] Quit", { labelX, 375.0f }, 16.0f, COLOR_UNDERTALE_GRAY, 1.0f);
}

} // namespace nundertale
