#include "scenes/LoreScene.hpp"
#include "scenes/TitleScene.hpp"
#include "core/GameContext.hpp"
#include "core/Constants.hpp"
#include "core/SceneManager.hpp"
#include "services/MusicTracks.hpp"
#include <raylib.h>
#include <cstdio>

namespace nundertale {

static constexpr size_t SLIP_CARD = 8; // card 9: the slip (SFX + longer pause)

void LoreScene::onEnter(GameContext& context) {
    const MusicEntry track = musicFor("lore");
    context.audio.playMusic(track.path, true, track.loopStart, track.loopEnd);

    // FRT-002 / LORE §9: the 10 canonical intro cards (gamer propaganda, biased narrator)
    m_cards = {
        { "assets/img/lore1.bmp", { "Long ago, two races ruled", "over the internet:", "GAMERS and FURRIES." } },
        { "assets/img/lore2.bmp", { "One day, the FURRIES tried", "to turn every GAMER", "into a FURRY." } },
        { "assets/img/lore3.bmp", { "The GAMERS fought back with", "copypastas, raids, and a", "total lack of shame." } },
        { "assets/img/lore4.bmp", { "After a decisive ratio,", "the GAMERS were victorious." } },
        { "assets/img/lore4.bmp", { "Seven GAMERS sealed the", "degenerates underground", "with a magic BARRIER." } },
        { "assets/img/lore5.bmp", { "Many years later...", "", "MT. DEW, 202X" } },
        { "assets/img/lore6.bmp", { "Legend says that those who", "keep on yiffin'", "may never game again..." } },
        // Card 8: Pepe looking at the Rift
        { "assets/img/lore7.bmp", {} },
        // Card 9: a foot slipping on a Dorito next to a tail
        { "assets/img/lore8.bmp", {} },
        // Card 10: the fall (followed by the lore10 animation and the cut to the title)
        { "assets/img/lore9.bmp", {} }
    };

    m_currentCard = 0;
    m_currentLine = 0;
    m_charIndex = 0;
    m_charTimer = 0.0f;
    m_cardTimer = 0.0f;
    m_cardAlpha = 0.0f;
    m_cardFadingIn = true;
    m_cardTextDone = false;
    m_isPlayingCard10Anim = false;
    m_card10Frame = 0;
    m_card10Timer = 0.0f;
}

void LoreScene::update(float dt, GameContext& context) {
    // Fast skip to Title on Cancel key
    if (context.input.isCancelPressed()) {
        context.audio.playSound("assets/sfx/cartoon_slip.ogg", 0.7f);
        context.scenes.changeScene(std::make_unique<TitleScene>(), 0.5f);
        return;
    }

    // Card 10 animation state
    if (m_isPlayingCard10Anim) {
        m_card10Timer += dt;
        if (m_card10Timer >= 0.05f) {
            m_card10Timer = 0.0f;
            m_card10Frame += 2; // Advance animation smoothly
            if (m_card10Frame >= CARD10_TOTAL_FRAMES) {
                context.scenes.changeScene(std::make_unique<TitleScene>(), 0.6f);
            }
        }
        if (context.input.isConfirmPressed()) {
            context.scenes.changeScene(std::make_unique<TitleScene>(), 0.5f);
        }
        return;
    }

    if (m_cardFadingIn) {
        m_cardAlpha += dt * 3.0f;
        if (m_cardAlpha >= 1.0f) {
            m_cardAlpha = 1.0f;
            m_cardFadingIn = false;

            // Trigger slip sound effect on card 8
            if (m_currentCard == SLIP_CARD) {
                context.audio.playSound("assets/sfx/cartoon_slip.ogg", 0.9f);
            }
        }
        return;
    }

    LoreCard& card = m_cards[m_currentCard];

    // If card has no text lines (cards 7, 8, 9), wait short timer or confirm to advance
    if (card.lines.empty()) {
        m_cardTimer += dt;
        float delay = (m_currentCard == SLIP_CARD) ? 2.0f : 1.5f; // the slip card has a slightly longer pause
        if (m_cardTimer >= delay || context.input.isConfirmPressed()) {
            m_currentCard++;
            if (m_currentCard < m_cards.size()) {
                m_cardTimer = 0.0f;
                m_cardAlpha = 0.0f;
                m_cardFadingIn = true;
                m_currentLine = 0;
                m_charIndex = 0;
                m_cardTextDone = false;
            } else {
                // Begin Card 10 animation
                m_isPlayingCard10Anim = true;
                m_card10Frame = 0;
                m_card10Timer = 0.0f;
            }
        }
        return;
    }

    // Card has text
    if (!m_cardTextDone) {
        m_charTimer += dt;
        if (m_charTimer >= 0.04f) {
            m_charTimer = 0.0f;
            if (m_currentLine < card.lines.size()) {
                const std::string& line = card.lines[m_currentLine];
                if (m_charIndex < line.size()) {
                    char c = line[m_charIndex];
                    m_charIndex++;
                    if (c != ' ' && c != '\t') {
                        context.audio.playSound("assets/sfx/type.ogg", 0.35f);
                    }
                } else {
                    m_currentLine++;
                    m_charIndex = 0;
                }
            } else {
                m_cardTextDone = true;
                m_cardTimer = 0.0f;
            }
        }

        // Advance line instantly on confirm
        if (context.input.isConfirmPressed()) {
            m_currentLine = card.lines.size();
            m_cardTextDone = true;
            m_cardTimer = 0.0f;
        }
    } else {
        m_cardTimer += dt;
        if (m_cardTimer >= 3.0f || context.input.isConfirmPressed()) {
            m_currentCard++;
            if (m_currentCard < m_cards.size()) {
                m_currentLine = 0;
                m_charIndex = 0;
                m_charTimer = 0.0f;
                m_cardTimer = 0.0f;
                m_cardAlpha = 0.0f;
                m_cardFadingIn = true;
                m_cardTextDone = false;
            } else {
                // Begin Card 10 animation
                m_isPlayingCard10Anim = true;
                m_card10Frame = 0;
                m_card10Timer = 0.0f;
            }
        }
    }
}

void LoreScene::render(GameContext& context) {
    ClearBackground(BLACK);

    Font font8Bit = context.resources.getFont8Bit(24);

    if (m_isPlayingCard10Anim) {
        // Draw Card 10 animation frame
        char pathBuf[80];
        int frameNum = (m_card10Frame >= CARD10_TOTAL_FRAMES) ? (CARD10_TOTAL_FRAMES - 1) : m_card10Frame;
        std::snprintf(pathBuf, sizeof(pathBuf), "assets/img/lore10/lore10__000%03d.bmp", frameNum);

        Texture2D animTex = context.resources.getTexture(pathBuf);
        DrawTexture(animTex, 121, 62, WHITE);
        context.resources.drawCenteredText(font8Bit, "[Press Z or X to Skip]", 440.0f, 14.0f, COLOR_UNDERTALE_GRAY, 1.0f);
        return;
    }

    if (m_currentCard >= m_cards.size()) return;

    LoreCard& card = m_cards[m_currentCard];
    Texture2D tex = context.resources.getTexture(card.imagePath);

    // Exact original coordinates: lore_img_posx = 121, lore_img_posy = 62
    unsigned char alphaByte = static_cast<unsigned char>(m_cardAlpha * 255.0f);
    Color tint = { 255, 255, 255, alphaByte };
    DrawTexture(tex, 121, 62, tint);

    // Exact original text coordinates: textx = 125, text1y = 310, text2y = 350, text3y = 390
    int textX = 125;
    int textYs[3] = { 310, 350, 390 };

    for (size_t i = 0; i < card.lines.size() && i < 3; ++i) {
        if (i < m_currentLine) {
            context.resources.drawText(font8Bit, card.lines[i], { (float)textX, (float)textYs[i] }, 24, WHITE, 1.0f);
        } else if (i == m_currentLine) {
            std::string sub = card.lines[i].substr(0, m_charIndex);
            context.resources.drawText(font8Bit, sub, { (float)textX, (float)textYs[i] }, 24, WHITE, 1.0f);
        }
    }

    // Skip helper
    context.resources.drawCenteredText(font8Bit, "[Z: Next / X: Skip to Menu]", 445.0f, 14.0f, COLOR_UNDERTALE_GRAY, 1.0f);
}

void LoreScene::onExit(GameContext& /*context*/) {
}

} // namespace nundertale
