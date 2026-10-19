#include "scenes/EncounterTransitionScene.hpp"
#include "core/GameContext.hpp"
#include "core/Constants.hpp"
#include "core/SceneManager.hpp"
#include "ui/SpriteDraw.hpp"

namespace nundertale {

EncounterTransitionScene::EncounterTransitionScene(Vector2 playerPos, std::unique_ptr<IScene> targetScene)
    : m_startPos({ playerPos.x + 14.0f, playerPos.y + 22.0f }),
      m_currentPos(m_startPos),
      m_targetScene(std::move(targetScene)) {}

void EncounterTransitionScene::onEnter(GameContext& context) {
    context.audio.stopMusic();
    m_phase = Phase::Alert;
    m_timer = 0.0f;
    m_flashStep = 0;
    m_soulVisible = false;
    context.audio.playSound("assets/sfx/alert.ogg", 0.9f);
}

void EncounterTransitionScene::update(float dt, GameContext& context) {
    m_timer += dt;

    if (m_phase == Phase::Alert) {
        // ENC-001: Exclamation bubble stays for ~15 frames (~0.45s)
        if (m_timer >= 0.45f) {
            m_phase = Phase::Flash;
            m_timer = 0.0f;
            m_flashStep = 0;
            m_soulVisible = true;
            context.audio.playSound("assets/sfx/encounter_beep.ogg", 0.9f);
        }
    } else if (m_phase == Phase::Flash) {
        if (m_timer >= 0.08f) {
            m_timer = 0.0f;
            m_flashStep++;
            m_soulVisible = (m_flashStep % 2 == 0);

            if (m_soulVisible) {
                context.audio.playSound("assets/sfx/encounter_beep.ogg", 0.9f);
            }

            if (m_flashStep >= 6) {
                m_phase = Phase::Slide;
                m_timer = 0.0f;
                m_soulVisible = true;
                context.audio.playSound("assets/sfx/encounter_fall.ogg", 0.6f);
            }
        }
    } else if (m_phase == Phase::Slide) {
        float t = m_timer / 0.35f;
        if (t >= 1.0f) {
            t = 1.0f;
            m_phase = Phase::Complete;
            // Pop transition and launch battle scene (ENC-002)
            auto battle = std::move(m_targetScene);
            context.scenes.popScene(context);
            context.scenes.pushScene(std::move(battle), context);
            return;
        }

        // Ease-in quadratic slide down to FIGHT button
        float ease = t * t;
        m_currentPos.x = m_startPos.x + (m_targetPos.x - m_startPos.x) * ease;
        m_currentPos.y = m_startPos.y + (m_targetPos.y - m_startPos.y) * ease;
    }
}

void EncounterTransitionScene::drawSoul(float x, float y) const {
    DrawRectangle((int)x + 2, (int)y, 4, 3, SOUL_RED);
    DrawRectangle((int)x + 8, (int)y, 4, 3, SOUL_RED);
    DrawRectangle((int)x + 1, (int)y + 3, 12, 4, SOUL_RED);
    DrawRectangle((int)x + 2, (int)y + 7, 10, 3, SOUL_RED);
    DrawRectangle((int)x + 4, (int)y + 10, 6, 2, SOUL_RED);
    DrawRectangle((int)x + 6, (int)y + 12, 2, 2, SOUL_RED);
}

void EncounterTransitionScene::render(GameContext& context) {
    ClearBackground(BLACK);

    if (m_phase == Phase::Alert) {
        // Draw player character (Pepe)
        Vector2 pPos = { m_startPos.x - 14.0f, m_startPos.y - 22.0f };
        const SpriteSheetDef* pepeSheet = spriteSheetCache().load(
            "assets/sprites/ow/pepe_frog.png",
            "assets/sprites/ow/pepe_frog.json"
        );
        if (pepeSheet != nullptr) {
            Texture2D pepeTexture = context.resources.getTexture("assets/sprites/ow/pepe_frog.png");
            SetTextureFilter(pepeTexture, TEXTURE_FILTER_POINT);
            drawSprite(
                pepeTexture,
                *pepeSheet,
                "stand_down",
                0.0f,
                { pPos.x + 19.0f, pPos.y + 58.0f }
            );
        } else {
            Texture2D pepeTexture = context.resources.getMaskedTexture("assets/img/pepe_char.bmp", { 255, 0, 255, 255 });
            Rectangle src = { 1, 1, 19, 29 };
            Rectangle dest = { pPos.x, pPos.y, 38, 58 };
            DrawTexturePro(pepeTexture, src, dest, { 0, 0 }, 0.0f, WHITE);
        }

        // Draw "!" exclamation balloon above player's head (ENC-001)
        float balloonX = pPos.x + 9.0f;
        float balloonY = pPos.y - 30.0f;
        if (FileExists("assets/ui/exclaim.png")) {
            Texture2D exclaimTexture = context.resources.getTexture("assets/ui/exclaim.png");
            SetTextureFilter(exclaimTexture, TEXTURE_FILTER_POINT);
            DrawTextureEx(
                exclaimTexture,
                { balloonX, balloonY + 4.0f },
                0.0f,
                2.0f,
                WHITE
            );
        } else {
            Rectangle bRect = { balloonX, balloonY, 20.0f, 22.0f };
            DrawRectangleRec(bRect, WHITE);
            DrawRectangleLinesEx(bRect, 2.0f, BLACK);

            // Little speech tail pointing down to head
            DrawTriangle(
                { balloonX + 6.0f, balloonY + 22.0f },
                { balloonX + 14.0f, balloonY + 22.0f },
                { balloonX + 10.0f, balloonY + 28.0f },
                WHITE
            );
            DrawLineEx({ balloonX + 6.0f, balloonY + 22.0f }, { balloonX + 10.0f, balloonY + 28.0f }, 2.0f, BLACK);
            DrawLineEx({ balloonX + 14.0f, balloonY + 22.0f }, { balloonX + 10.0f, balloonY + 28.0f }, 2.0f, BLACK);

            // "!" mark
            DrawRectangle(static_cast<int>(balloonX) + 8, static_cast<int>(balloonY) + 4, 4, 9, BLACK);
            DrawRectangle(static_cast<int>(balloonX) + 8, static_cast<int>(balloonY) + 15, 4, 3, BLACK);
        }
    } else {
        if (m_soulVisible) {
            drawSoul(m_currentPos.x, m_currentPos.y);
        }
    }
}

void EncounterTransitionScene::onExit(GameContext& /*context*/) {
}

} // namespace nundertale
