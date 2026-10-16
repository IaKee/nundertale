#include "ui/DialogBox.hpp"
#include "core/GameContext.hpp"
#include "core/Constants.hpp"

namespace nundertale {

DialogBox::DialogBox() = default;

void DialogBox::startDialog(const std::vector<std::string>& pages, const std::string& speaker, std::function<void()> onComplete) {
    m_pages = pages;
    m_speaker = speaker;
    m_onComplete = onComplete;
    m_currentPage = 0;
    m_visibleChars = 0;
    m_charTimer = 0.0f;
    m_isActive = true;
    m_isPageComplete = false;
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

    const std::string& currentText = m_pages[m_currentPage];

    if (!m_isPageComplete) {
        m_charTimer += dt;
        if (m_charTimer >= m_charDelay) {
            m_charTimer = 0.0f;
            if (m_visibleChars < currentText.size()) {
                char c = currentText[m_visibleChars];
                m_visibleChars++;
                if (c != ' ' && c != '\n' && c != '\t') {
                    context.audio.playSound("assets/sfx/type.ogg", 0.4f);
                }
            } else {
                m_isPageComplete = true;
            }
        }

        // Fast-forward on confirm or cancel
        if (context.input.isConfirmPressed() || context.input.isCancelPressed()) {
            m_visibleChars = currentText.size();
            m_isPageComplete = true;
        }
    } else {
        // Page is complete, waiting for confirm to advance or close
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

    // Draw dialog frame
    DrawRectangleRec(m_boxRect, BLACK);
    DrawRectangleLinesEx(m_boxRect, 4.0f, WHITE);

    // Draw speaker name if present
    if (!m_speaker.empty()) {
        std::string tag = "* " + m_speaker + ":";
        context.resources.drawText(font8Bit, tag, { m_boxRect.x + 20, m_boxRect.y + 12 }, 20, COLOR_UNDERTALE_YELLOW, 1.0f);
    }

    // Draw visible text
    const std::string& text = m_pages[m_currentPage];
    std::string visibleText = text.substr(0, m_visibleChars);

    float textOffsetY = m_speaker.empty() ? 20.0f : 38.0f;
    context.resources.drawText(font8Bit, visibleText, { m_boxRect.x + 25, m_boxRect.y + textOffsetY }, 22, WHITE, 1.0f);

    // Blinking prompt when waiting for advance
    if (m_isPageComplete) {
        if ((int)(GetTime() * 3.0) % 2 == 0) {
            DrawTriangle(
                { m_boxRect.x + m_boxRect.width - 25, m_boxRect.y + m_boxRect.height - 15 },
                { m_boxRect.x + m_boxRect.width - 15, m_boxRect.y + m_boxRect.height - 15 },
                { m_boxRect.x + m_boxRect.width - 20, m_boxRect.y + m_boxRect.height - 8 },
                COLOR_UNDERTALE_YELLOW
            );
        }
    }
}

} // namespace nundertale
