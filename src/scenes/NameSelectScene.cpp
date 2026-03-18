#include "scenes/NameSelectScene.hpp"
#include "scenes/OverworldScene.hpp"
#include "scenes/TitleScene.hpp"
#include "core/GameContext.hpp"
#include "core/Constants.hpp"
#include "core/SceneManager.hpp"
#include <raylib.h>
#include <algorithm>

namespace nundertale {

static const char* MATRIX[4][7] = {
    { "A", "B", "C", "D", "E", "F", "G" },
    { "H", "I", "J", "K", "L", "M", "N" },
    { "O", "P", "Q", "R", "S", "T", "U" },
    { "V", "W", "X", "Y", "Z", "PRO", "GAMER" }
};

void NameSelectScene::onEnter(GameContext& /*context*/) {
    m_state = NameSelectState::Selecting;
    m_currentName.clear();
    m_cursorX = 0;
    m_cursorY = 0;
    m_confirmChoice = true;
    m_animTimer = 0.0f;
    m_subtitle = "";
    m_metaMessage = "";
    m_isChad = false;
    m_isSkyrim = false;
    m_isWaifu = false;
    m_isUndertaleMeta = false;
    m_isIllegal = false;
    m_tooManyAttempts = 0;
}

void NameSelectScene::checkEasterEggs(GameContext& context) {
    m_isChad = (m_currentName == "CHAD");
    m_isSkyrim = (m_currentName == "SKYRIM");
    m_isWaifu = (m_currentName == "RUIVA" || m_currentName == "LOIRA" || m_currentName == "OTOME" ||
                 m_currentName == "GOTICA" || m_currentName == "MORENA" || m_currentName == "YANDER" ||
                 m_currentName == "EDUCAD" || m_currentName == "RMLOLI");
    m_isUndertaleMeta = (m_currentName == "SANS" || m_currentName == "FRISK" || m_currentName == "CHARA" ||
                         m_currentName == "FLOWEY" || m_currentName == "TORIEL" || m_currentName == "ASGORE" ||
                         m_currentName == "UNDYNE" || m_currentName == "ALPHYS");

    bool allSame = !m_currentName.empty();
    for (size_t i = 1; i < m_currentName.size(); ++i) {
        if (m_currentName[i] != m_currentName[0]) {
            allSame = false;
            break;
        }
    }

    if (m_isChad) {
        m_subtitle = "[GOLD]";
        m_metaMessage = "Gold plated. You are the ultimate gamer.";
        context.audio.playSound("assets/sfx/u_activate.wav", 1.0f);
    } else if (m_currentName == "PEPE") {
        m_subtitle = "[THE PROTAGONIST]";
        m_metaMessage = "Feels good man. Ready to climb down Mt. Dew.";
        context.audio.playSound("assets/sfx/cartoon_slip.ogg", 0.8f);
    } else if (m_isSkyrim) {
        m_subtitle = "[TAMRIEL]";
        m_metaMessage = "Hey you, you're finally awake...";
        context.audio.playSound("assets/sfx/u_activate.wav", 0.9f);
    } else if (m_currentName == "SANS") {
        m_subtitle = "[COMEDIAN]";
        m_metaMessage = "heh, you really think you have the backbone for this?";
        context.audio.playSound("assets/sfx/sans_type.wav", 1.0f);
    } else if (m_currentName == "FRISK") {
        m_subtitle = "[FALLEN CHILD]";
        m_metaMessage = "Warning: This mode triggers extreme allergy to fursuits.";
    } else if (m_currentName == "CHARA") {
        m_subtitle = "[THE FIRST]";
        m_metaMessage = "The true meaning of pure toxic gamer rage.";
    } else if (m_currentName == "FLOWEY") {
        m_subtitle = "[WEED]";
        m_metaMessage = "Not very original, are you?";
    } else if (m_currentName == "TORIEL") {
        m_subtitle = "[MOM]";
        m_metaMessage = "Furiell has already trademarked this name in the Ruins.";
    } else if (m_currentName == "ASGORE") {
        m_subtitle = "[KING]";
        m_metaMessage = "King of questionable herbal tea.";
    } else if (m_isWaifu) {
        m_subtitle = "[WAIFU APPRECIATOR]";
        m_metaMessage = "A true cultured gamer of fine 2D taste.";
        context.audio.playSound("assets/sfx/u_activate.wav", 0.8f);
    } else if (allSame && m_currentName.size() > 1) {
        m_subtitle = "";
        m_metaMessage = "Not very creative... Did your keyboard jam?";
    } else if (m_currentName.size() == 1) {
        m_subtitle = "";
        m_metaMessage = "You call that a gamer? Saving characters?";
    } else {
        m_subtitle = "";
        m_metaMessage = "";
    }
}

void NameSelectScene::update(float dt, GameContext& context) {
    m_animTimer += dt;

    if (m_state == NameSelectState::Selecting) {
        // Grid navigation (4 rows x 7 cols, plus bottom control row Y=4)
        if (context.input.isLeftPressed()) {
            m_cursorX = (m_cursorX + 6) % 7;
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
        } else if (context.input.isRightPressed()) {
            m_cursorX = (m_cursorX + 1) % 7;
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
        } else if (context.input.isUpPressed()) {
            m_cursorY = (m_cursorY + 4) % 5;
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
        } else if (context.input.isDownPressed()) {
            m_cursorY = (m_cursorY + 1) % 5;
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.4f);
        }

        // Backspace
        if (context.input.isBackspacePressed() && !m_currentName.empty()) {
            m_currentName.pop_back();
            context.audio.playSound("assets/sfx/type.ogg", 0.5f);
            checkEasterEggs(context);
        }

        // Confirm letter or bottom buttons
        if (context.input.isConfirmPressed()) {
            if (m_cursorY < 4) {
                // Letter matrix
                const char* entry = MATRIX[m_cursorY][m_cursorX];
                if (m_currentName.size() < 10) {
                    m_currentName += entry;
                    context.audio.playSound("assets/sfx/type.ogg", 0.6f);
                    checkEasterEggs(context);
                } else {
                    m_tooManyAttempts++;
                    context.audio.playSound("assets/sfx/areusure.ogg", 0.6f);
                }
            } else {
                // Bottom row: Left half is Backspace, Right half is Done
                if (m_cursorX < 3) {
                    // Backspace
                    if (!m_currentName.empty()) {
                        m_currentName.pop_back();
                        context.audio.playSound("assets/sfx/type.ogg", 0.5f);
                        checkEasterEggs(context);
                    }
                } else {
                    // Done
                    if (!m_currentName.empty()) {
                        context.audio.playSound("assets/sfx/u_activate.wav", 0.8f);
                        m_state = NameSelectState::Confirming;
                        m_confirmChoice = true;
                    }
                }
            }
        }

        if (context.input.isEscapePressed()) {
            context.scenes.changeScene(std::make_unique<TitleScene>(), 0.3f);
        }

    } else if (m_state == NameSelectState::Confirming) {
        if (context.input.isLeftPressed() || context.input.isRightPressed()) {
            m_confirmChoice = !m_confirmChoice;
            context.audio.playSound("assets/sfx/selection_sound.wav", 0.5f);
        }

        if (context.input.isConfirmPressed()) {
            if (m_confirmChoice) {
                // Confirm name and start game!
                context.player.name = m_currentName;
                context.player.isChad = m_isChad;
                context.player.isSkyrim = m_isSkyrim;
                context.player.isWaifuFan = m_isWaifu;
                context.audio.playSound("assets/sfx/u_activate.wav", 1.0f);
                context.scenes.changeScene(std::make_unique<OverworldScene>(), 0.8f);
            } else {
                // Return to editing
                m_state = NameSelectState::Selecting;
                context.audio.playSound("assets/sfx/selection_sound.wav", 0.5f);
            }
        } else if (context.input.isCancelPressed()) {
            m_state = NameSelectState::Selecting;
        }
    }
}

void NameSelectScene::render(GameContext& context) {
    ClearBackground(BLACK);

    Font font8Bit = context.resources.getFont8Bit(24);
    Font fontTitle = context.resources.getFontTitleFore(32);

    if (m_state == NameSelectState::Selecting) {
        // If Skyrim easter egg, show background image
        if (m_isSkyrim) {
            Texture2D skyrimTex = context.resources.getTexture("assets/img/whoru.bmp");
            DrawTexturePro(skyrimTex, { 0, 0, (float)skyrimTex.width, (float)skyrimTex.height },
                           { 0, 0, (float)VIRTUAL_WIDTH, (float)VIRTUAL_HEIGHT }, { 0, 0 }, 0.0f, { 255, 255, 255, 70 });
        }

        // Title: "Name the fallen gamer." in authentic Undertale font
        context.resources.drawCenteredText(font8Bit, "Name the fallen gamer.", 35, 28, WHITE, 2.0f);

        // Name box & current name
        Color nameColor = m_isChad ? COLOR_UNDERTALE_YELLOW : (m_isWaifu ? Color{ 255, 105, 180, 255 } : WHITE);
        std::string displayName = m_currentName.empty() ? "_" : m_currentName;
        context.resources.drawCenteredText(font8Bit, displayName, 95, 36, nameColor, 3.0f);

        // Subtitle / Tag (e.g. [GOLD])
        if (!m_subtitle.empty()) {
            context.resources.drawCenteredText(font8Bit, m_subtitle, 138, 20, COLOR_UNDERTALE_YELLOW, 2.0f);
        }

        // Meta easter egg comment
        if (!m_metaMessage.empty()) {
            context.resources.drawCenteredText(font8Bit, m_metaMessage, 162, 16, COLOR_UNDERTALE_CYAN, 1.0f);
        } else if (m_tooManyAttempts > 0) {
            const char* msg = (m_tooManyAttempts > 4) ? "Stop it... Get some help..." : "You only need 6-8 characters...";
            context.resources.drawCenteredText(font8Bit, msg, 162, 16, COLOR_UNDERTALE_RED, 1.0f);
        }

        // Character matrix with authentic Undertale jitter and spacing
        int startX = 89;
        int startY = 200;
        int spacingX = 70;
        int spacingY = 40;

        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 7; ++c) {
                bool isSelected = (m_cursorY == r && m_cursorX == c);
                Color letterColor = isSelected ? COLOR_UNDERTALE_YELLOW : WHITE;

                // Subtle authentic jitter via drawShakyText (GFX-009)
                const char* label = MATRIX[r][c];
                Vector2 charPos = { (float)(startX + c * spacingX), (float)(startY + r * spacingY) };

                context.resources.drawShakyText(font8Bit, label, charPos, 24, letterColor, 1.2f, 1.0f);

                if (isSelected) {
                    DrawRectangleLines((int)charPos.x - 6, (int)charPos.y - 4, 38, 28, COLOR_UNDERTALE_YELLOW);
                }
            }
        }

        // Bottom row: [ Quit ] [ Backspace ] [ Done ]
        bool isBackSelected = (m_cursorY == 4 && m_cursorX < 3);
        bool isDoneSelected = (m_cursorY == 4 && m_cursorX >= 3);

        Color backColor = isBackSelected ? COLOR_UNDERTALE_YELLOW : COLOR_UNDERTALE_GRAY;
        Color doneColor = isDoneSelected ? COLOR_UNDERTALE_YELLOW : (m_currentName.empty() ? COLOR_UNDERTALE_DARK : WHITE);

        context.resources.drawText(font8Bit, "[ Backspace ]", { 140.0f, 380.0f }, 22, backColor, 1.0f);
        context.resources.drawText(font8Bit, "[ Done ]", { 410.0f, 380.0f }, 22, doneColor, 1.0f);

        // Footer helper
        context.resources.drawCenteredText(font8Bit, "[ARROWS] Move   [Z] Select   [X] Delete   [ESC] Quit", 440, 15, COLOR_UNDERTALE_GRAY, 1.0f);

    } else if (m_state == NameSelectState::Confirming) {
        // Confirmation screen (faithful to name_confirm in nundertale.c)
        context.resources.drawCenteredText(font8Bit, "Are you sure about that?", 80, 30, WHITE, 2.0f);

        // Subtitle commentary
        if (!m_metaMessage.empty()) {
            context.resources.drawCenteredText(font8Bit, m_metaMessage, 130, 20, COLOR_UNDERTALE_CYAN, 1.0f);
        } else if (m_currentName.size() == 1) {
            context.resources.drawCenteredText(font8Bit, "You call that a gamer?", 130, 20, WHITE, 1.0f);
        } else {
            context.resources.drawCenteredText(font8Bit, "Nice name bro.", 130, 20, WHITE, 1.0f);
        }

        // Center name with animated rotation & scale
        Color nameColor = m_isChad ? COLOR_UNDERTALE_YELLOW : (m_isWaifu ? Color{ 255, 105, 180, 255 } : WHITE);
        context.resources.drawCenteredText(fontTitle, m_currentName, 210, 48, nameColor, 3.0f);

        if (m_isChad) {
            context.resources.drawCenteredText(font8Bit, "[GOLD PLATED GAMER]", 270, 20, COLOR_UNDERTALE_YELLOW, 2.0f);
        }

        // YES / NO choices
        Color yesColor = m_confirmChoice ? COLOR_UNDERTALE_YELLOW : COLOR_UNDERTALE_GRAY;
        Color noColor = !m_confirmChoice ? COLOR_UNDERTALE_YELLOW : COLOR_UNDERTALE_GRAY;

        context.resources.drawText(font8Bit, "YES", { 220.0f, 340.0f }, 28, yesColor, 2.0f);
        context.resources.drawText(font8Bit, "NO", { 380.0f, 340.0f }, 28, noColor, 2.0f);

        int heartX = m_confirmChoice ? 190 : 350;
        DrawCircle(heartX, 354, 7, COLOR_UNDERTALE_GREEN);
    }
}

void NameSelectScene::onExit(GameContext& /*context*/) {
}

} // namespace nundertale
