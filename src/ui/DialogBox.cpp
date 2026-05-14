#include "ui/DialogBox.hpp"
#include "core/GameContext.hpp"
#include "core/Constants.hpp"
#include <sstream>

namespace nundertale {

DialogBox::DialogBox() = default;

void DialogBox::startDialog(const std::vector<std::string>& pages, const std::string& speaker, std::function<void()> onComplete, const std::string& voiceSfx) {
    m_pages.clear();
    m_pageOptions.clear();
    for (const auto& rawPage : pages) {
        auto parsed = parseChoiceToken(rawPage);
        m_pages.push_back(parsed.text);
        m_pageOptions.push_back(parsed.options);
    }
    m_speaker = speaker;
    m_onComplete = onComplete;
    m_onChoice = nullptr;
    m_lastChoice = -1;
    m_selectedChoice = 0;
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

void DialogBox::setOnChoice(std::function<void(int index, const std::string& option)> cb) {
    m_onChoice = cb;
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
    const std::vector<std::string>& currentOptions = (m_currentPage < m_pageOptions.size()) ? m_pageOptions[m_currentPage] : std::vector<std::string>{};
    bool hasChoices = !currentOptions.empty();

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
                    m_visibleChars = 0;
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
                    m_visibleChars = 0;
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
