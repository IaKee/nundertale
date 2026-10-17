# B1 · `assets/sprites/battle/barky.png` — Barky — batalha (mini-boss)

> Ficha autocontida: tudo o que é preciso para gerar e entregar esta imagem. Índice e regras gerais em [README.md](README.md).

| Campo | Valor |
| :--- | :--- |
| Arquivo final | `assets/sprites/battle/barky.png` |
| JSON | `assets/sprites/battle/barky.json` |
| Prioridade | P0 — Ruins · **núcleo da vertical slice** |
| Onde aparece | R9 — Barky's Arena (fundo: grade verde + um tubo fluorescente no topo). |
| O código já lê? | **Sim** — `BattleSystem` carrega `barky.png` + `barky.json` e usa `idle`, `hurt`, `blush` (após Cringe Check) e `spared` (desenhado translúcido). Sem os arquivos, usa as primitivas. |

## Envio (resumo)

Conversa nova no gerador → enviar **5 anexos**: `B01_battle_barky.md` + `templates/B01_battle_barky.png` + `cards/B01_battle_barky.png` + `assets/sprites/ow/barky.png` + *Undertale — Toriel battle sprite* → colar o prompt da seção 7. Detalhes de cada anexo na seção 6.

## 1. Contexto

Barky the Alpha Wolf (ver também A4). Intro: `Halt, hairless intruder! *[awkward howling]* You have stepped into the domain of the Shadow Fang Pack! Prepare to face my fursona!`

Check: `* BARKY - ATK 4 DEF 2` / `* Alpha Wolf of the Ruins. The zipper says otherwise.`
ACTs relevantes para as poses: **Cringe Check** (encara o zíper → `H-hey! It's an artisanal design feature, b-baka!` → pose `blush`), Offer Dew, Debate Lore, Laser, **Accept Hug** (`...this is the best day of my life. Don't tell anyone.`).

**Visual:** orelhas grandes pontudas, franja emo cobrindo um olho, jaqueta com tachas, correntes de plástico, luvas sem dedo, rabo com alfinete, **zíper visível**. A LORE pede o Barky "em partes" (orelhas, jaqueta, correntes, rabo) para animar como os inimigos do Undertale no futuro.

## 2. Regras que valem para esta imagem

- **Escala:** arte em **1× nativo** (tela cheia = 320×240). O jogo desenha a **2×** com filtro *point* num canvas virtual de 640×480. Todos os tamanhos desta ficha são 1×.
- **Pixel:** sem anti-aliasing, sem gradiente suave, sem blur, sem sub-pixel.
- **Sprite sheet:** grade fixa, células do mesmo tamanho, sem espaçamento nem margem. **Uma linha por animação**, frames da esquerda para a direita **começando na coluna 0**. O loader não sabe começar em outra coluna, então **estados/variantes diferentes vão em linhas diferentes**. Célula vazia = transparente.
- **Pivô (`origin`):** o mesmo ponto (pé / base central) em todas as células.
- **Conteúdo:** nada explícito; proporções **adultas**; nenhum rosto real; nenhuma marca real (nada de logo Doritos / Mountain Dew; use "CITRUS BLAST", chips sem marca); todo texto dentro da arte em **inglês**.
- **Undertale:** pode servir de referência de tamanho/estilo, mas nada copiado vai para o repositório.
- Batalha é **branco sobre preto**: traço e detalhes `#FFFFFF`, interior da silhueta **preto opaco** `#000000`.
- Fora da silhueta = **transparente**. O fundo das batalhas comuns tem uma grade verde que não pode aparecer através do personagem, por isso o interior precisa ser preto opaco.
- Nada de cinza: "meio-tom" se faz com pixels brancos espaçados.
- Cor só em momentos especiais, e só onde esta ficha disser.

## 3. Formato de output esperado

- **Tipo:** PNG RGBA 32-bit, fundo transparente.
- **Célula (um frame):** 60×80 px.
- **Grade:** 2 coluna(s) × 4 linha(s).
- **Tamanho total do PNG:** **120×320 px** (1× nativo; o jogo desenha a 2×).
- **Pivô (`origin`):** [30, 80] dentro de cada célula.
- `cell` e `origin` já são os documentados em `assets/sprites/README.md` — não mude.
- Entregue também o arquivo de trabalho (`.aseprite`/`.psd`) com cada parte numa camada (orelhas, cabeça, jaqueta, correntes, rabo).
- **Do gerador de imagem:** peça em alta resolução com fundo `#FF00FF`; o arquivo entregue é o pequeno, já reduzido e recortado (ver "Do gerador ao arquivo final").

**Montagem do sheet:**

| Linha | Animação | Frames | Detalhe |
| :---: | :--- | :--- | :--- |
| 0 | `idle` | 2 | pose edgy de braços cruzados; frame 2: orelha esquerda dobra 2 px e o rabo vai para o outro lado |
| 1 | `hurt` | 1 | olhos fechados apertados, orelhas achatadas, recuando |
| 2 | `blush` | 1 | corando, olhando de lado, patas tapando o zíper — **única cor permitida:** bochechas `#FF77C8` |
| 3 | `spared` | 1 | relaxado, sorriso tímido |

## 4. Paleta

| Uso | Hex |
| :--- | :--- |
| Traço | `#FFFFFF` |
| Preenchimento interno (opaco) | `#000000` |
| Blush (só linha 2) | `#FF77C8` |

## 5. Referência atual

Primitivas em `src/combat/BattleSystem.cpp` (blocos de corpo, cabeça, orelhas e rabo) — só referência de proporção/partes.

## 6. Pacote de envio (até 5 anexos)

Numa conversa nova, anexe **nesta ordem** e cole o prompt da seção 7:

| # | Anexo | Onde está | Para quê |
| :---: | :--- | :--- | :--- |
| md | Esta ficha | [`B01_battle_barky.md`](B01_battle_barky.md) | especificação completa |
| 1 | Template da grade/canvas | [`templates/B01_battle_barky.png`](templates/B01_battle_barky.png) | layout exato: proporção, linhas, colunas, pivô |
| 2 | Cartão de estilo | [`cards/B01_battle_barky.png`](cards/B01_battle_barky.png) | densidade de pixel (âncora), paleta, tamanho e regras numa imagem |
| 3 | Saída aprovada da ficha A4 | `assets/sprites/ow/barky.png` (depois de aprovada) | mesmo personagem / mesmo design |
| 4 | Undertale — Toriel battle sprite | The Spriters Resource → Undertale (só referência: salve em `assets/_ref/`, git-ignored) | estilo do traço branco sobre preto |

**Alternativas** (troque uma imagem acima, ou use como 5ª se o gerador não contar o `.md`):

- Foto/ilustração real: emo scene kid 2008 outfit — busca de imagens: "emo scene kid 2008 outfit" — franja e roupa
- Foto/ilustração real: wolf partial fursuit head — busca de imagens: "wolf partial fursuit head" — orelhas de lobo

Referências são **só entrada**: material do Undertale ou de terceiros fica em `assets/_ref/` (git-ignored) e nunca é commitado nem copiado pixel a pixel.

## 7. Prompt (autocontido)

Cole inteiro. Ele repete as regras de estilo, a paleta e o formato, e explica cada anexo. Se não enviar alguma imagem, apague a linha dela em "Attached".

```text
You are drawing game art for nUndertale, a parody of the game Undertale. Rules: true pixel art in Undertale's style; every art pixel is one uniform square block; 1-pixel dark outline; flat colors with at most one shadow tone per color; no anti-aliasing, gradients, blur, painted texture or semi-transparent edges, including where the drawing meets the background. Match the pixel density of the anchor sprite on the style card: a human-sized overworld character is about 20x30 art pixels. Render the image large, but keep that number of art pixels. Use only the hex colors listed in this prompt. Adult proportions, nothing explicit, no real people, no brand logos, no text except exact words given here, no watermark or signature.

Attached: the spec file B01_battle_barky.md (full details) and these images, in order:
- image 1: layout template: copy its canvas proportions, rows, columns and cells exactly (one drawing per gray cell, white cells empty, feet or base on the red line); never draw its grid, gray cells, red marks or labels
- image 2: style card: match its pixel density (anchor sprite), palette, size and rules; never draw the card itself; the frog on it is only a pixel-density example, do not draw a frog or copy its features
- image 3: the approved design of the same character: keep its design exactly
- image 4: reference for white-on-black line style only
Draw a new image; never copy the reference images.

Undertale battle sprite, pure white 1-pixel line art on a solid black fill, monochrome (only #FFFFFF and #000000), bold readable silhouette, hand-drawn pixel art, no anti-aliasing, no gray, no gradients, front-facing, centered, isolated on a solid flat #FF00FF magenta background, no watermark. Battle sprite sheet of an edgy man in a cheap wolf fursuit: big pointy ears, an emo fringe covering one eye, a studded leather jacket, plastic chains, fingerless gloves, a fluffy tail held by a safety pin, a costume zipper visible at the side of the neck. Half-body to full-body, front view, each frame in an equal 60x80 cell. Strict grid of 4 rows: row 1 idle with arms crossed (2 frames, ear twitch and tail sway), row 2 hurt (squeezed-shut eyes, flattened ears, recoiling), row 3 embarrassed, blushing, looking away and covering the zipper with his paws, row 4 relaxed shy smile. Only exception to monochrome: small pink #FF77C8 blush marks in row 3. No text.

Use only these colors: #FFFFFF (Traço), #000000 (Preenchimento interno), #FF77C8 (Blush).
Output: one image with the whole sheet laid out exactly like image 1 (2 columns x 4 rows of equal cells), on a flat, exact #FF00FF magenta background with no noise.
Avoid: blurry, anti-aliased, soft shading, smooth gradient, 3D render, realistic, photo, painterly, noisy texture, random dithering, jpeg artifacts, watermark, signature, brand logo, real person, real face, child proportions, baby face, nsfw, suggestive, cropped, cut off, inconsistent sizes between frames.
```

## 8. JSON

Salvar como `assets/sprites/battle/barky.json`. `frames` padrão 1, `fps` padrão 4, `loop` padrão `true`; animação sem loop congela no último frame; anim inexistente cai em `idle`.

```json
{
  "image": "barky.png",
  "cell": [60, 80],
  "origin": [30, 80],
  "anims": {
    "idle": { "row": 0, "frames": 2, "fps": 2 },
    "hurt": { "row": 1, "frames": 1 },
    "blush": { "row": 2, "frames": 1 },
    "spared": { "row": 3, "frames": 1 }
  }
}
```

## 9. Do gerador ao arquivo final

1. Numa conversa nova do gerador, envie o pacote da seção 6 (o `.md` + até 4 imagens) e cole o prompt da seção 7.
2. Se o gerador errar a grade, gere **uma pose/frame por vez** (comece pelo parado de frente), aprove, e use o frame aprovado como referência para os outros; monte o sheet à mão. Alternativa: geradores nativos de pixel art (PixelLab, Retro Diffusion para Aseprite) já entregam no tamanho real.
3. No Aseprite / LibreSprite / Pixelorama: reduza para o tamanho nativo com **nearest neighbor** (ou redesenhe por cima), quantize para a paleta da seção 4, limpe o contorno para 1 px.
4. Monte na grade da seção 3, alinhe o pivô com uma guia, troque o magenta por transparência.
5. Exporte no formato da seção 3, escreva o JSON (seção "JSON", quando houver) e registre o arquivo em `assets/PLACEHOLDERS.md` (fonte/gerador, autor, licença).

**Específico desta imagem:**

- Garanta preto **opaco** em todo o interior (teste sobre uma grade verde).
- Pés/base tocando y=80 em todas as células.

## 10. Checklist de aceite

- [ ] O PNG tem exatamente 120×320 px
- [ ] Sem anti-aliasing e sem cores fora da paleta da seção 4
- [ ] Fundo transparente (magenta removido)
- [ ] JSON válido ao lado do PNG, com os nomes de animação da seção 3
- [ ] No jogo: idle anima, hurt aparece ao acertar, blush após Cringe Check, spared translúcido.
- [ ] Nenhum pixel cinza.
- [ ] Registrado em `assets/PLACEHOLDERS.md` se não for a arte final
