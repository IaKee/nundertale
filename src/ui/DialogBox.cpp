#include "ui/DialogBox.hpp"
#include "core/GameContext.hpp"
#include "core/Constants.hpp"
#include <sstream>

namespace nundertale {

DialogBox::DialogBox() = default;

void DialogBox::startDialog(const std::vector<std::string>& pages, const std::string& speaker, std::function<void()> onComplete, const std::string& voiceSfx) {
    m_pages = pages;
    m_speaker = speaker;
    m_onComplete = onComplete;
    m_currentPage = 0;
    m_visibleChars = 0;
    m_charTimer = 0.0f;
    m_isActive = true;
    m_isPageComplete = false;

    // TXT-006: Voice per character
    if (!voiceSfx.empty()) {
        m_voiceSfx = voiceSfx;
    } else if (speaker.find("Sans") != std::string::npos) {
        m_voiceSfx = "assets/sfx/sans_type.wav";
    } else if (speaker.find("Discord") != std::string::npos || speaker.find("Raid") != std::string::npos) {
        m_voiceSfx = "assets/sfx/type_long.ogg";
    } else {
        m_voiceSfx = "assets/sfx/type.ogg";
    }
}

void DialogBox::close() {
    m_isActive = false;
    if (m_onComplete) {
        auto cb = m_onComplete;
        m_onComplete = nullptr;
        cb();
    }
}

void DialogBox::update(float dt, GameContext& context) {
    if (!m_isActive || m_pages.empty()) return;

    // TXT-005: Dialog box position flips to top if player is in lower half of screen
    if (context.state.playerPos.y > 240.0f) {
        m_boxRect = { 32.0f, 15.0f, 576.0f, 150.0f };
    } else {
        m_boxRect = { 32.0f, 315.0f, 576.0f, 150.0f };
    }

    const std::string& currentText = m_pages[m_currentPage];

    if (!m_isPageComplete) {
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
    } else {
        // Page is complete, Z (Confirm) advances to the next page or closes (TXT-004)
        if (context.input.isConfirmPressed()) {
            m_currentPage++;
            if (m_currentPage < m_pages.size()) {
                m_visibleChars = 0;
                m_charTimer = 0.0f;
                m_isPageComplete = false;
            } else {
                close();
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

    // Undertale 1:1: No yellow "* SPEAKER:" header (TXT-005)
    // Draw text with word wrapping and hanging indent (TXT-002)
    const std::string& fullText = m_pages[m_currentPage];
    std::string text = fullText.substr(0, m_visibleChars);

    float startX = m_boxRect.x + 25.0f;
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
                context.resources.drawText(font8Bit, currentLine, { effectiveX, currentY }, 22, WHITE, 1.0f);
                currentY += lineH;
                firstLineOfPara = false;
                currentLine = word;
            } else {
                currentLine = testLine;
            }
        }

        if (!currentLine.empty()) {
            float effectiveX = (firstLineOfPara || !hasAsterisk) ? startX : indentX;
            context.resources.drawText(font8Bit, currentLine, { effectiveX, currentY }, 22, WHITE, 1.0f);
            currentY += lineH;
        }
    }
}

} // namespace nundertale
