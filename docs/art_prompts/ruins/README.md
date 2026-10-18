# Ruins — prompts de arte (P0)

Uma ficha por imagem que o level inicial (Ruins, salas R1–R13) precisa. Cada ficha é **autocontida**: no topo diz exatamente o que enviar ao gerador (o `.md` + até 4 imagens = 5 anexos) e traz contexto, regras, formato de output, paleta, referências, um prompt autocontido, JSON e checklist de aceite. Passo a passo em [WORKFLOW.md](WORKFLOW.md).

Fontes: `docs/ASSETS_NEEDED.md` §4.1–§4.7, `docs/LORE.md` (§9, §10, §12, §18.1, §19, §25) e o loader de sprite sheets (`include/core/SpriteSheet.hpp`, `assets/sprites/README.md`). Áudio não entra aqui.

**★ = núcleo da vertical slice** (ASSETS_NEEDED §7). **O código já lê** hoje só `sprites/ow/pepe_frog`, `sprites/battle/barky` e `img/lore4.bmp`; o resto já está no formato do loader e aparece quando as cenas forem ligadas a ele.

## Kit de identidade visual

Cada ficha usa duas imagens próprias: `templates/<ficha>.png` (grade/canvas exato) e `cards/<ficha>.png` (cartão de estilo: densidade de pixel do âncora + paleta da ficha + tamanho + regras, tudo numa imagem para caber no limite de 5 anexos). As imagens gerais abaixo são para consulta e servem de referência alternativa.

| Arquivo | Para que serve |
| :--- | :--- |
| [`STYLE_GUIDE.md`](STYLE_GUIDE.md) | guia de identidade visual: direção de arte, modos, escala, paletas, personagens, regras |
| [`WORKFLOW.md`](WORKFLOW.md) | como montar cada chamada ao gerador (5 anexos) e em que ordem gerar |
| [`style/01_palettes.png`](style/01_palettes.png) | todas as paletas com hex |
| [`style/02_rendering_rules.png`](style/02_rendering_rules.png) | regras de render com exemplos BOM / RUIM |
| [`style/03_scale_lineup.png`](style/03_scale_lineup.png) | escala relativa de todos os personagens e objetos |
| [`style/04_pepe_anchor.png`](style/04_pepe_anchor.png) | sprite-âncora: densidade de pixels, contorno e sombreamento de referência |
| [`style/04_pepe_anchor_native.png`](style/04_pepe_anchor_native.png) | o mesmo sprite-âncora no tamanho real (24×32) |
| [`style/05_pepe_legacy.png`](style/05_pepe_legacy.png) | sprite antigo do Pepe (identidade) |
| `templates/<ficha>.png` | grade/canvas exato de cada ficha (anexo 1) |
| `cards/<ficha>.png` | cartão de estilo de cada ficha (anexo 2) |

**Fluxo:** abrir a ficha → bloco **Envio** no topo → conversa nova com o `.md` + as imagens listadas → colar o prompt da seção 7 → pós-processar (seção "Do gerador ao arquivo final"). Detalhes em [WORKFLOW.md](WORKFLOW.md).

Para regerar fichas, templates e kit depois de mudar algo: `python tools/art_prompts/gen.py` (na raiz do repositório).

## Regras gerais

- **Escala:** arte em **1× nativo** (tela cheia = 320×240). O jogo desenha a **2×** com filtro *point* num canvas virtual de 640×480. Todos os tamanhos desta ficha são 1×.
- **Pixel:** sem anti-aliasing, sem gradiente suave, sem blur, sem sub-pixel.
- **Sprite sheet:** grade fixa, células do mesmo tamanho, sem espaçamento nem margem. **Uma linha por animação**, frames da esquerda para a direita **começando na coluna 0**. O loader não sabe começar em outra coluna, então **estados/variantes diferentes vão em linhas diferentes**. Célula vazia = transparente.
- **Pivô (`origin`):** o mesmo ponto (pé / base central) em todas as células.
- **Conteúdo:** nada explícito; proporções **adultas**; nenhum rosto real; nenhuma marca real (nada de logo Doritos / Mountain Dew; use "CITRUS BLAST", chips sem marca); todo texto dentro da arte em **inglês**.
- **Undertale:** pode servir de referência de tamanho/estilo, mas nada copiado vai para o repositório.

## Problemas encontrados nas referências atuais

| Arquivo | Problema | Ficha |
| :--- | :--- | :--- |
| `assets/img/lore4.bmp` | Rosto real (meme) e logo Doritos | G1 |
| `assets/img/test.bmp` | É a foto "Bliss" do Windows XP (copyright), usada como fundo do overworld | F1 substitui |
| `assets/img/pepe_char.bmp` | Rosto simplificado (sem pálpebras nem dois lábios) | A1 segue o meme: camiseta azul + traços do rosto (já é o canon na LORE) |
| `assets/sprites/player.png` | 41×43, um traço preto sem relação com o Pepe | ignorar / remover |


## A. Personagens — overworld

| # | Ficha | Arquivo final | Tamanho do PNG | Código lê? | Núcleo |
| :---: | :--- | :--- | :--- | :---: | :---: |
| A1 | [Pepe (protagonista) — overworld](A01_ow_pepe_frog.md) | `assets/sprites/ow/pepe_frog.png` | 96×128 (célula 24×32) | sim | ★ |
| A2 | [Pepe deitado / levantando (R1)](A02_ow_pepe_wakeup.md) | `assets/sprites/ow/pepe_wakeup.png` | 96×64 (célula 32×32) | não |  |
| A3 | [Furiell — overworld](A03_ow_furiell.md) | `assets/sprites/ow/furiell.png` | 128×220 (célula 32×44) | não |  |
| A4 | [Barky — overworld](A04_ow_barky.md) | `assets/sprites/ow/barky.png` | 112×180 (célula 28×36) | não | ★ |
| A5 | [Floofy — overworld](A05_ow_floofy.md) | `assets/sprites/ow/floofy.png` | 72×96 (célula 24×24) | não |  |

## B. Personagens — batalha

| # | Ficha | Arquivo final | Tamanho do PNG | Código lê? | Núcleo |
| :---: | :--- | :--- | :--- | :---: | :---: |
| B1 | [Barky — batalha (mini-boss)](B01_battle_barky.md) | `assets/sprites/battle/barky.png` | 120×320 (célula 60×80) | sim | ★ |
| B2 | [Furiell — batalha (boss)](B02_battle_furiell.md) | `assets/sprites/battle/furiell.png` | 160×330 (célula 80×110) | não |  |
| B3 | [Floofy — batalha (tutorial)](B03_battle_floofy.md) | `assets/sprites/battle/floofy.png` | 80×120 (célula 40×40) | não |  |
| B4 | [Fursuit Dummy — batalha (tutorial)](B04_battle_fursuit_dummy.md) | `assets/sprites/battle/fursuit_dummy.png` | 80×120 (célula 40×60) | não |  |
| B5 | [Froggsona — batalha (inimigo comum, paródia de Froggit)](B05_battle_froggsona.md) | `assets/sprites/battle/froggsona.png` | 80×80 (célula 40×40) | não |  |
| B6 | [Shysona — batalha (inimigo comum, paródia de Whimsun)](B06_battle_shysona.md) | `assets/sprites/battle/shysona.png` | 80×80 (célula 40×40) | não |  |
| B7 | [Modsmal — batalha (inimigo comum, paródia de Moldsmal)](B07_battle_modsmal.md) | `assets/sprites/battle/modsmal.png` | 80×60 (célula 40×30) | não |  |
| B8 | [Lurkeye — batalha (inimigo comum, paródia de Loox)](B08_battle_lurkeye.md) | `assets/sprites/battle/lurkeye.png` | 100×100 (célula 50×50) | não |  |
| B9 | [Reply Guy — batalha (inimigo comum, paródia de Migosp)](B09_battle_reply_guy.md) | `assets/sprites/battle/reply_guy.png` | 80×112 (célula 40×56) | não |  |
| B10 | [Grasstoid — batalha (inimigo comum, paródia de Vegetoid)](B10_battle_grasstoid.md) | `assets/sprites/battle/grasstoid.png` | 100×120 (célula 50×60) | não |  |

## C. Retratos de diálogo

| # | Ficha | Arquivo final | Tamanho do PNG | Código lê? | Núcleo |
| :---: | :--- | :--- | :--- | :---: | :---: |
| C1 | [Retratos de diálogo — Furiell](C01_portraits_furiell.md) | `assets/portraits/furiell_*.png` | 5 × 50×50 | não |  |
| C2 | [Retratos de diálogo — Floofy](C02_portraits_floofy.md) | `assets/portraits/floofy_*.png` | 3 × 50×50 | não |  |
| C3 | [Retratos de diálogo — Barky](C03_portraits_barky.md) | `assets/portraits/barky_*.png` | 3 × 50×50 | não |  |

## D. Projéteis

| # | Ficha | Arquivo final | Tamanho do PNG | Código lê? | Núcleo |
| :---: | :--- | :--- | :--- | :---: | :---: |
| D1 | [Projétil `heart_pellet` — Floofy (R2) e o padrão `hug_ring` da Furiell](D01_bullet_heart_pellet.md) | `assets/bullets/heart_pellet.png` | 16×8 (célula 8×8) | não |  |
| D2 | [Projétil `uwu_letters` — Barky — padrão `uwu_rain`](D02_bullet_uwu_letters.md) | `assets/bullets/uwu_letters.png` | 12×28 (célula 12×14) | não |  |
| D3 | [Projétil `can` — Barky — padrão `dew_bounce`](D03_bullet_can.md) | `assets/bullets/can.png` | 56×14 (célula 14×14) | não |  |
| D4 | [Projétil `sound_ring` — Barky — `howl_wave`; Lurkeye — `seen_rings`](D04_bullet_sound_ring.md) | `assets/bullets/sound_ring.png` | 32×96 (célula 32×32) | não |  |
| D5 | [Projétil `fluff_ball` — Furiell — padrão `fluff_balls`](D05_bullet_fluff_ball.md) | `assets/bullets/fluff_ball.png` | 24×12 (célula 12×12) | não |  |
| D6 | [Projétil `glitter_hand` — Furiell — padrão `glitter_hands`](D06_bullet_glitter_hand.md) | `assets/bullets/glitter_hand.png` | 60×30 (célula 30×30) | não |  |
| D7 | [Projétil `colon_three` — Froggsona — padrão `colon_three_hop`](D07_bullet_colon_three.md) | `assets/bullets/colon_three.png` | 24×12 (célula 12×12) | não |  |
| D8 | [Projétil `fly` — Froggsona — padrão `fly_swarm`](D08_bullet_fly.md) | `assets/bullets/fly.png` | 16×8 (célula 8×8) | não |  |
| D9 | [Projétil `moth` — Shysona — padrão `sorry_moths`](D09_bullet_moth.md) | `assets/bullets/moth.png` | 24×10 (célula 12×10) | não |  |
| D10 | [Projétil `gel_hammer` — Modsmal — padrão `ban_gel`](D10_bullet_gel_hammer.md) | `assets/bullets/gel_hammer.png` | 24×16 (célula 12×16) | não |  |
| D11 | [Projétil `text_bug` — Reply Guy — padrão `well_actually`](D11_bullet_text_bug.md) | `assets/bullets/text_bug.png` | 32×8 (célula 16×8) | não |  |
| D12 | [Projétil `grass_blade` — Grasstoid — padrão `grass_blades`](D12_bullet_grass_blade.md) | `assets/bullets/grass_blade.png` | 16×32 (célula 8×16) | não |  |
| D13 | [Projétil `tear` — Napstablurk — padrão `seen_tears` (opcional, P2)](D13_bullet_tear.md) | `assets/bullets/tear.png` | 6×8 (célula 6×8) | não |  |

## E. Interface

| # | Ficha | Arquivo final | Tamanho do PNG | Código lê? | Núcleo |
| :---: | :--- | :--- | :--- | :---: | :---: |
| E1 | [Botões de batalha FIGHT / ACT / ITEM / MERCY](E01_ui_buttons.md) | `assets/ui/buttons.png` | 55×189 (célula 55×21) | não | ★ |
| E2 | [Almas (SOUL) do jogador](E02_ui_souls.md) | `assets/ui/souls.png` | 12×72 (célula 12×12) | não | ★ |
| E3 | [Mira do FIGHT (target)](E03_ui_target.md) | `assets/ui/target.png` | 280×65 | não |  |
| E4 | [Barra de ataque do FIGHT](E04_ui_attack_bar.md) | `assets/ui/attack_bar.png` | 14×65 (célula 7×65) | não |  |
| E5 | [Golpe (hitmarker de FPS)](E05_ui_slash.md) | `assets/ui/slash.png` | 180×30 (célula 30×30) | não |  |
| E6 | ["!" do encontro](E06_ui_exclaim.md) | `assets/ui/exclaim.png` | 10×12 | não |  |
| E7 | [Logo "FURRY OVER"](E07_ui_furry_over.md) | `assets/ui/furry_over.png` | 210×90 | não |  |
| E8 | [Logo do título ("UNDERTALE" sem o n)](E08_ui_logo.md) | `assets/ui/logo.png` | 200×30 | não |  |
| E9 | [Logo do título — o "n" vermelho](E09_ui_logo_n.md) | `assets/ui/logo_n.png` | 14×20 | não |  |
| E10 | [Ponto de save (estrela na garrafa)](E10_props_save_point.md) | `assets/props/save_point.png` | 160×28 (célula 20×28) | não | ★ |
| E11 | [Peças do HUD (opcional)](E11_ui_hud.md) | `assets/ui/hud.png` | 60×10 | não |  |
| E12 | [Caixa de diálogo (opcional)](E12_ui_dialog_box.md) | `assets/ui/dialog_box.png` | 289×76 | não |  |
| E13 | [Moldura do retrato (opcional)](E13_ui_portrait_frame.md) | `assets/ui/portrait_frame.png` | 52×52 | não |  |

## F. Tiles e props

| # | Ficha | Arquivo final | Tamanho do PNG | Código lê? | Núcleo |
| :---: | :--- | :--- | :--- | :---: | :---: |
| F1 | [Tileset das Ruins](F01_tiles_ruins.md) | `assets/tiles/ruins.png` | 160×160 (célula 20×20) | não | ★ |
| F2 | [Banner principal da convenção](F02_props_ruins_banner_welcome.md) | `assets/props/ruins_banner_welcome.png` | 60×40 | não |  |
| F3 | [Faixa "Deodorant is NOT optional"](F03_props_ruins_banner_deodorant.md) | `assets/props/ruins_banner_deodorant.png` | 60×16 | não |  |
| F4 | [Tenda de vendedor abandonada](F04_props_ruins_booth.md) | `assets/props/ruins_booth.png` | 60×40 | não |  |
| F5 | [Cortina 18+ da Art Show](F05_props_ruins_curtain_18.md) | `assets/props/ruins_curtain_18.png` | 40×40 | não |  |
| F6 | [Monitor CRT com tela azul](F06_props_ruins_crt.md) | `assets/props/ruins_crt.png` | 40×20 (célula 20×20) | não |  |
| F7 | [Mural de avisos](F07_props_ruins_notice_board.md) | `assets/props/ruins_notice_board.png` | 40×30 | não |  |
| F8 | [Interruptor RGB](F08_props_ruins_rgb_switch.md) | `assets/props/ruins_rgb_switch.png` | 20×60 (célula 20×20) | não |  |
| F9 | [Tigela de chips](F09_props_ruins_dorito_bowl.md) | `assets/props/ruins_dorito_bowl.png` | 20×60 (célula 20×20) | não |  |
| F10 | [Pilha de embalagens de salgadinho](F10_props_ruins_wrapper_pile.md) | `assets/props/ruins_wrapper_pile.png` | 40×40 (célula 40×20) | não |  |
| F11 | [Cabos que dão choque (os "espinhos")](F11_props_ruins_cables.md) | `assets/props/ruins_cables.png` | 40×40 (célula 20×20) | não |  |
| F12 | [Lareira](F12_props_ruins_fireplace.md) | `assets/props/ruins_fireplace.png` | 120×40 (célula 40×40) | não |  |
| F13 | [Poltrona felpuda](F13_props_ruins_armchair.md) | `assets/props/ruins_armchair.png` | 30×30 | não |  |
| F14 | [Cama do quarto do Pepe](F14_props_ruins_bed.md) | `assets/props/ruins_bed.png` | 40×30 | não |  |
| F15 | [Armário (com a Fursuit Head)](F15_props_ruins_closet.md) | `assets/props/ruins_closet.png` | 30×80 (célula 30×40) | não |  |
| F16 | [Espelho do corredor](F16_props_ruins_mirror.md) | `assets/props/ruins_mirror.png` | 20×80 (célula 20×40) | não |  |
| F17 | [Estante](F17_props_ruins_bookshelf.md) | `assets/props/ruins_bookshelf.png` | 40×40 | não |  |
| F18 | [Luminária de globo ("a lua do Barky")](F18_props_ruins_lamp.md) | `assets/props/ruins_lamp.png` | 40×40 (célula 20×40) | não |  |

## G. Card de intro

| # | Ficha | Arquivo final | Tamanho do PNG | Código lê? | Núcleo |
| :---: | :--- | :--- | :--- | :---: | :---: |
| G1 | [Card de intro "BEGONE!" (redesenho obrigatório)](G01_img_lore4.md) | `assets/img/lore4.bmp` | 398×214 | sim | ★ |

## Depois de gerar

1. Coloque o PNG (e o JSON) no caminho final em `assets/`.
2. Registre em `assets/PLACEHOLDERS.md` (fonte/gerador, autor, licença). Para imagens de IA, anote o serviço e confira os termos de uso comercial.
3. Teste o que o código já lê: `pepe_frog` (andar nas 4 direções, pivô nos pés), `battle/barky` (idle, hurt, blush após Cringe Check, spared) e o card `lore4`.
