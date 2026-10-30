#include "ui/DialogBox.hpp"
#include "core/GameContext.hpp"
#include "core/Constants.hpp"
#include "ui/SpriteDraw.hpp"
#include <algorithm>
#include <sstream>

namespace nundertale {

void DialogBox::update(float dt, GameContext& context) {
    if (!m_isActive || m_pages.empty()) return;

    m_fur = context.state.furry.value;
    applyFurToPages();

    // TXT-005: Dialog box position flips to top if player is in lower half of screen
    if (context.state.playerPos.y > 240.0f) {
        m_boxRect = { 32.0f, 15.0f, 576.0f, 150.0f };
    } else {
        m_boxRect = { 32.0f, 315.0f, 576.0f, 150.0f };
    }

    const std::string& currentText = m_pages[m_currentPage];
    const std::vector<std::string>& currentOptions = (m_currentPage < m_pageOptions.size()) ? m_pageOptions[m_currentPage] : std::vector<std::string>{};
    bool hasChoices = !currentOptions.empty();

    if (!m_isPageComplete) {
        if (m_reveal == "fade_glow") {
            m_revealAlpha = std::min(1.0f, m_revealAlpha + std::max(0.0f, dt) / FADE_GLOW_SECONDS);
            m_visibleChars = currentText.size();
            if (m_revealAlpha >= 1.0f) m_isPageComplete = true;
            if (context.input.isCancelPressed()) {
                m_revealAlpha = 1.0f;
                m_isPageComplete = true;
            }
        } else {
            m_charTimer += dt;
            if (m_charTimer >= m_charDelay) {
                m_charTimer = 0.0f;
                if (m_visibleChars < currentText.size()) {
                    char c = currentText[m_visibleChars];
                    m_visibleChars++;

                    // Voice SFX on non-whitespace
                    if (c != ' ' && c != '\n' && c != '\t') {
                        context.audio.playSound(m_voiceSfx, 0.45f);
                    }

                    // TXT-003: Punctuation pauses
                    if (c == ',' || c == ':') {
                        m_charTimer = -0.12f;
                    } else if (c == '.' || c == '!' || c == '?') {
                        m_charTimer = -0.28f;
                    }
                } else {
                    m_isPageComplete = true;
                }
            }

            // BUG-11 / TXT-004: Only X (Cancel) skips typing instantly; Z waits for page completion
            if (context.input.isCancelPressed()) {
                m_visibleChars = currentText.size();
                m_isPageComplete = true;
            }
        }
    } else {
        if (hasChoices) {
            // TXT-007: Left/Right navigate options with selection sound
            int numOpts = static_cast<int>(currentOptions.size());
            if (context.input.isLeftPressed()) {
                if (m_selectedChoice > 0) {
                    m_selectedChoice--;
                    context.audio.playSound("assets/sfx/menu_move.wav", 0.4f);
                }
            } else if (context.input.isRightPressed()) {
                if (m_selectedChoice + 1 < numOpts) {
                    m_selectedChoice++;
                    context.audio.playSound("assets/sfx/menu_move.wav", 0.4f);
                }
            }

            // TXT-007: X does nothing on a choice page once typed (no cancel)

            // TXT-007: Z confirms choice, invokes callback, then advances
            if (context.input.isConfirmPressed()) {
                m_lastChoice = m_selectedChoice;
                if (m_onChoice) {
                    m_onChoice(m_selectedChoice, currentOptions[m_selectedChoice]);
                }
                m_currentPage++;
                if (m_currentPage < m_pages.size()) {
                    m_revealAlpha = m_reveal == "fade_glow" ? 0.0f : 1.0f;
                    m_visibleChars = m_reveal == "fade_glow" ? m_pages[m_currentPage].size() : 0;
                    m_charTimer = 0.0f;
                    m_isPageComplete = false;
                    m_selectedChoice = 0;
                } else {
                    close();
                }
            }
        } else {
            // Page is complete, Z (Confirm) advances to the next page or closes (TXT-004)
            if (context.input.isConfirmPressed()) {
                m_currentPage++;
                if (m_currentPage < m_pages.size()) {
                    m_revealAlpha = m_reveal == "fade_glow" ? 0.0f : 1.0f;
                    m_visibleChars = m_reveal == "fade_glow" ? m_pages[m_currentPage].size() : 0;
                    m_charTimer = 0.0f;
                    m_isPageComplete = false;
                    m_selectedChoice = 0;
                } else {
                    close();
                }
            }
        }
    }
}

void DialogBox::render(GameContext& context) {
    if (!m_isActive || m_pages.empty()) return;

    Font font8Bit = context.resources.getFont8Bit(22);

    // TXT-005: Draw dialog frame (white border 5px, black fill)
    DrawRectangleRec(m_boxRect, BLACK);
    DrawRectangleLinesEx(m_boxRect, 5.0f, WHITE);

    bool portraitDrawn = false;
    if (m_portrait.has_value()) {
        Texture2D texture = context.resources.getTexture(m_portrait->imagePath);
        if (texture.id > 0) {
            SetTextureFilter(texture, TEXTURE_FILTER_POINT);
            SpriteSheetDef sheet = m_portrait->sheet;
            if (sheet.cellW <= 0 || sheet.cellH <= 0) {
                sheet.cellW = texture.width;
                sheet.cellH = texture.height;
                sheet.originX = 0;
                sheet.originY = 0;
            }
            const float drawScale = portraitDrawScale(sheet.density);
            const Vector2 topLeft = { m_boxRect.x + 18.0f, m_boxRect.y + 25.0f };
            const Vector2 pivot = {
                topLeft.x + static_cast<float>(sheet.originX) * drawScale,
                topLeft.y + static_cast<float>(sheet.originY) * drawScale
            };
            drawSprite(texture, sheet, "idle", 0.0f, pivot, 2.0f);
            portraitDrawn = true;
        }
    }

    // Undertale 1:1: No yellow "* SPEAKER:" header (TXT-005)
    // Draw text with word wrapping and hanging indent (TXT-002)
    const std::string& fullText = m_pages[m_currentPage];
    std::string text = fullText.substr(0, m_visibleChars);

    float startX = m_boxRect.x + 25.0f + (portraitDrawn ? 112.0f : 0.0f);
    float startY = m_boxRect.y + 22.0f;
    float lineH = 32.0f;
    float indentX = startX + 24.0f;

    std::stringstream ss(text);
    std::string paragraph;
    float currentY = startY;

    while (std::getline(ss, paragraph, '\n')) {
        if (paragraph.empty()) {
            currentY += lineH * 0.5f;
            continue;
        }

        bool hasAsterisk = (paragraph.rfind("* ", 0) == 0);
        std::stringstream wordStream(paragraph);
        std::string word;
        std::string currentLine = "";
        bool firstLineOfPara = true;

        while (wordStream >> word) {
            std::string testLine = currentLine.empty() ? word : (currentLine + " " + word);
            float effectiveX = (firstLineOfPara || !hasAsterisk) ? startX : indentX;
            float availableW = m_boxRect.x + m_boxRect.width - 25.0f - effectiveX;

            float testW = context.resources.measureTextWidth(font8Bit, testLine, 22, 1.0f);
            if (testW > availableW && !currentLine.empty()) {
                if (m_reveal == "fade_glow") {
                    const unsigned char alpha = static_cast<unsigned char>(m_revealAlpha * 255.0f);
                    const Color glow = { 255, 176, 48, static_cast<unsigned char>(alpha / 3) };
                    context.resources.drawText(font8Bit, currentLine, { effectiveX - 1.0f, currentY }, 22, glow, 1.0f);
                    context.resources.drawText(font8Bit, currentLine, { effectiveX + 1.0f, currentY }, 22, glow, 1.0f);
                    context.resources.drawText(font8Bit, currentLine, { effectiveX, currentY }, 22, Color{ 255, 202, 96, alpha }, 1.0f);
                } else {
                    context.resources.drawText(font8Bit, currentLine, { effectiveX, currentY }, 22, WHITE, 1.0f);
                }
                currentY += lineH;
                firstLineOfPara = false;
                currentLine = word;
            } else {
                currentLine = testLine;
            }
        }

        if (!currentLine.empty()) {
            float effectiveX = (firstLineOfPara || !hasAsterisk) ? startX : indentX;
            if (m_reveal == "fade_glow") {
                const unsigned char alpha = static_cast<unsigned char>(m_revealAlpha * 255.0f);
                const Color glow = { 255, 176, 48, static_cast<unsigned char>(alpha / 3) };
                context.resources.drawText(font8Bit, currentLine, { effectiveX - 1.0f, currentY }, 22, glow, 1.0f);
                context.resources.drawText(font8Bit, currentLine, { effectiveX + 1.0f, currentY }, 22, glow, 1.0f);
                context.resources.drawText(font8Bit, currentLine, { effectiveX, currentY }, 22, Color{ 255, 202, 96, alpha }, 1.0f);
            } else {
                context.resources.drawText(font8Bit, currentLine, { effectiveX, currentY }, 22, WHITE, 1.0f);
            }
            currentY += lineH;
        }
    }

    // TXT-007: Render options once page is complete
    const std::vector<std::string>& currentOptions = (m_currentPage < m_pageOptions.size()) ? m_pageOptions[m_currentPage] : std::vector<std::string>{};
    if (m_isPageComplete && !currentOptions.empty()) {
        float optionsY = std::max(currentY + 6.0f, m_boxRect.y + 100.0f);

        for (size_t i = 0; i < currentOptions.size(); ++i) {
            float optX = 0.0f;
            if (currentOptions.size() == 2) {
                optX = (i == 0) ? (m_boxRect.x + 80.0f) : (m_boxRect.x + 300.0f);
            } else {
                float spacing = (m_boxRect.width - 120.0f) / static_cast<float>(currentOptions.size());
                optX = m_boxRect.x + 60.0f + static_cast<float>(i) * spacing;
            }

            // Draw option text
            context.resources.drawText(font8Bit, currentOptions[i], { optX, optionsY }, 22, WHITE, 1.0f);

            // Red soul cursor to the left of the selected option
            if (static_cast<int>(i) == m_selectedChoice) {
                float soulX = optX - 24.0f;
                float soulY = optionsY + 4.0f;
                DrawRectangle(static_cast<int>(soulX) + 2, static_cast<int>(soulY), 4, 3, SOUL_RED);
                DrawRectangle(static_cast<int>(soulX) + 8, static_cast<int>(soulY), 4, 3, SOUL_RED);
                DrawRectangle(static_cast<int>(soulX) + 1, static_cast<int>(soulY) + 3, 12, 4, SOUL_RED);
                DrawRectangle(static_cast<int>(soulX) + 2, static_cast<int>(soulY) + 7, 10, 3, SOUL_RED);
                DrawRectangle(static_cast<int>(soulX) + 4, static_cast<int>(soulY) + 10, 6, 2, SOUL_RED);
                DrawRectangle(static_cast<int>(soulX) + 6, static_cast<int>(soulY) + 12, 2, 2, SOUL_RED);
            }
        }
    }
}

} // namespace nundertale
