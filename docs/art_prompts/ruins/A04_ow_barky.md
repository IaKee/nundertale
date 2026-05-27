# A4 · `assets/sprites/ow/barky.png` — Barky — overworld

> Ficha autocontida: tudo o que é preciso para gerar e entregar esta imagem. Índice e regras gerais em [README.md](README.md).

| Campo | Valor |
| :--- | :--- |
| Arquivo final | `assets/sprites/ow/barky.png` |
| JSON | `assets/sprites/ow/barky.json` |
| Prioridade | P0 — Ruins · **núcleo da vertical slice** |
| Onde aparece | R9 (arena; entrada caindo do teto) e depois como NPC se poupado. |
| O código já lê? | Não. Formato pronto para o loader (a entrada está em `data/cutscenes/barky_entrance.json`). |

## Envio (resumo)

Conversa nova no gerador → enviar **5 anexos**: `A04_ow_barky.md` + `templates/A04_ow_barky.png` + `cards/A04_ow_barky.png` + *Undertale — Frisk overworld sprite sheet* + *Foto/ilustração real: emo scene kid 2008 outfit* → colar o prompt da seção 7. Detalhes de cada anexo na seção 6.

## 1. Contexto

Barky the Alpha Wolf, mini-boss das Ruins. Ex-`xX_BarkSc0p3_Xx`, **23 anos**, o 7º convertido (2021). Edgy, dramático, uiva para lâmpadas fluorescentes, lidera a "Shadow Fang Pack" (membros: ele). Inseguro: acha que todo mundo percebe que ele é "novo". Tem flashbacks de gamer.

**Visual (LORE §10.3):** orelhas de lobo, jaqueta de couro falso com tachas, correntes de plástico, luvas sem dedo, **rabo preso com alfinete**, fursuit barata com **zíper visível**. **Silhueta:** as orelhas.

**Entrada (ROTEIRO cena 3):** um uivo, Barky cai do teto e aterrissa agachado.

## 2. Regras que valem para esta imagem

- **Escala:** arte em **1× nativo** (tela cheia = 320×240). O jogo desenha a **2×** com filtro *point* num canvas virtual de 640×480. Todos os tamanhos desta ficha são 1×.
- **Pixel:** sem anti-aliasing, sem gradiente suave, sem blur, sem sub-pixel.
- **Sprite sheet:** grade fixa, células do mesmo tamanho, sem espaçamento nem margem. **Uma linha por animação**, frames da esquerda para a direita **começando na coluna 0**. O loader não sabe começar em outra coluna, então **estados/variantes diferentes vão em linhas diferentes**. Célula vazia = transparente.
- **Pivô (`origin`):** o mesmo ponto (pé / base central) em todas as células.
- **Conteúdo:** nada explícito; proporções **adultas**; nenhum rosto real; nenhuma marca real (nada de logo Doritos / Mountain Dew; use "CITRUS BLAST", chips sem marca); todo texto dentro da arte em **inglês**.
- **Undertale:** pode servir de referência de tamanho/estilo, mas nada copiado vai para o repositório.
- Overworld é **colorido**, com paleta limitada (Ruins: roxo + verde neon).
- Vista 3/4 de cima, igual aos sprites de overworld do Undertale.
- Contorno escuro de 1 px; uma sombra por cor; sem anti-aliasing.

## 3. Formato de output esperado

- **Tipo:** PNG RGBA 32-bit, fundo transparente.
- **Célula (um frame):** 28×36 px.
- **Grade:** 4 coluna(s) × 5 linha(s).
- **Tamanho total do PNG:** **112×180 px** (1× nativo; o jogo desenha a 2×).
- **Pivô (`origin`):** [14, 35] dentro de cada célula.
- Personagem ~22×32; a folga em cima é para as orelhas.
- **Do gerador de imagem:** peça em alta resolução com fundo `#FF00FF`; o arquivo entregue é o pequeno, já reduzido e recortado (ver "Do gerador ao arquivo final").

**Montagem do sheet:**

| Linha | Animação | Col 0 | Col 1 | Col 2 | Col 3 |
| :---: | :--- | :--- | :--- | :--- | :--- |
| 0 | `walk_down` / `stand_down` | parado, de frente | passo pé esquerdo | parado (cópia da col 0) | passo pé direito |
| 1 | `walk_up` / `stand_up` | parado, de costas | passo esquerdo | parado (cópia) | passo direito |
| 2 | `walk_left` / `stand_left` | parado, perfil esquerdo | passo | *(vazio)* | *(vazio)* |
| 3 | `walk_right` / `stand_right` | parado, perfil direito | passo | *(vazio)* | *(vazio)* |
| 4 | `drop` (sem loop) | caindo, braços para cima, orelhas achatadas | aterrissagem agachado, punho no chão | levantando em pose dramática | *(vazio)* |

## 4. Paleta

| Uso | Hex |
| :--- | :--- |
| Contorno | `#141418` |
| Pelo / sombra | `#9A9AAA` / `#6E6E7E` |
| Interior das orelhas | `#C98AA8` |
| Jaqueta / realce | `#232329` / `#3C3C46` |
| Tachas, correntes plásticas | `#C8C8D2` |
| Luvas | `#232329` |
| Mecha emo na franja | `#7CFF3A` |
| Olhos (delineador) | `#D0263A` |
| Alfinete / zíper | `#9A9AAA` |

## 5. Referência atual

Nenhuma. Na batalha ele é desenhado com primitivas em `BattleSystem.cpp` (corpo, cabeça, orelhas, rabo) — só serve de referência de partes.

## 6. Pacote de envio (até 5 anexos)

Numa conversa nova, anexe **nesta ordem** e cole o prompt da seção 7:

| # | Anexo | Onde está | Para quê |
| :---: | :--- | :--- | :--- |
| md | Esta ficha | [`A04_ow_barky.md`](A04_ow_barky.md) | especificação completa |
| 1 | Template da grade/canvas | [`templates/A04_ow_barky.png`](templates/A04_ow_barky.png) | layout exato: proporção, linhas, colunas, pivô |
| 2 | Cartão de estilo | [`cards/A04_ow_barky.png`](cards/A04_ow_barky.png) | densidade de pixel (âncora), paleta, tamanho e regras numa imagem |
| 3 | Undertale — Frisk overworld sprite sheet | The Spriters Resource → Undertale (só referência: salve em `assets/_ref/`, git-ignored) | layout de 4 direções e ciclo de caminhada |
| 4 | Foto/ilustração real: emo scene kid 2008 outfit | busca de imagens: "emo scene kid 2008 outfit" | franja emo, roupa |

**Alternativas** (troque uma imagem acima, ou use como 5ª se o gerador não contar o `.md`):

- Foto/ilustração real: studded faux leather jacket — busca de imagens: "studded faux leather jacket" — jaqueta com tachas
- Foto/ilustração real: wolf partial fursuit head — busca de imagens: "wolf partial fursuit head" — orelhas e cabeça de lobo
- Foto/ilustração real: superhero landing pose — busca de imagens: "superhero landing pose" — pose de aterrissagem (linha `drop`)

Referências são **só entrada**: material do Undertale ou de terceiros fica em `assets/_ref/` (git-ignored) e nunca é commitado nem copiado pixel a pixel.

## 7. Prompt (autocontido)

Cole inteiro. Ele repete as regras de estilo, a paleta e o formato, e explica cada anexo. Se não enviar alguma imagem, apague a linha dela em "Attached".

```text
You are drawing game art for nUndertale, a parody of the game Undertale. Rules: true pixel art in Undertale's style; every art pixel is one uniform square block; 1-pixel dark outline; flat colors with at most one shadow tone per color; no anti-aliasing, gradients, blur, painted texture or semi-transparent edges, including where the drawing meets the background. Match the pixel density of the anchor sprite on the style card: a human-sized overworld character is about 20x30 art pixels. Render the image large, but keep that number of art pixels. Use only the hex colors listed in this prompt. Adult proportions, nothing explicit, no real people, no brand logos, no text except exact words given here, no watermark or signature.

Attached: the spec file A04_ow_barky.md (full details) and these images, in order:
- image 1: layout template: copy its canvas proportions, rows, columns and cells exactly (one drawing per gray cell, white cells empty, feet or base on the red line); never draw its grid, gray cells, red marks or labels
- image 2: style card: match its pixel density (anchor sprite), palette, size and rules; never draw the card itself; the frog on it is only a pixel-density example, do not draw a frog or copy its features
- image 3: reference for 4-direction layout and walk cycle only
- image 4: reference for emo fringe and outfit only
Draw a new image; never copy the reference images.

Pixel art sprite in the style of Undertale and Deltarune overworld sprites, 3/4 top-down RPG view, crisp 1-pixel dark outline, flat cel shading with one shadow tone per color, very limited palette, no anti-aliasing, no gradients, no blur, chunky readable pixels, adult proportions, isolated on a solid flat #FF00FF magenta background, no watermark. Character sprite sheet of an edgy 23-year-old man in a cheap gray wolf fursuit: huge pointy wolf ears, emo side-swept fringe with one neon green streak, red eyeliner eyes, black studded faux-leather jacket, plastic silver chains, fingerless gloves, a fluffy wolf tail attached with a big safety pin, a visible costume zipper on his back. Dramatic try-hard posture, arms crossed when idle. Strict grid of 5 rows, all frames the same size, feet on the same baseline: row 1 facing down (idle, left step, idle, right step), row 2 facing up showing the zipper and safety-pinned tail (idle, left step, idle, right step), row 3 facing left (idle, step), row 4 facing right (idle, step), row 5 entrance (falling from above with arms up, superhero landing crouch with one fist on the floor, standing up into a dramatic pose). Adult proportions, about 22x32 pixels per frame, the ears must read clearly as a black silhouette. No text.

Use only these colors: #141418 (Contorno), #9A9AAA (Pelo), #6E6E7E (sombra), #C98AA8 (Interior das orelhas), #232329 (Jaqueta), #3C3C46 (realce), #C8C8D2 (Tachas, correntes plásticas), #232329 (Luvas), #7CFF3A (Mecha emo na franja), #D0263A (Olhos), #9A9AAA (Alfinete / zíper).
Output: one image with the whole sheet laid out exactly like image 1 (4 columns x 5 rows of equal cells), on a flat, exact #FF00FF magenta background with no noise.
Avoid: blurry, anti-aliased, soft shading, smooth gradient, 3D render, realistic, photo, painterly, noisy texture, random dithering, jpeg artifacts, watermark, signature, brand logo, real person, real face, child proportions, baby face, nsfw, suggestive, cropped, cut off, inconsistent sizes between frames.
```

## 8. JSON

Salvar como `assets/sprites/ow/barky.json`. `frames` padrão 1, `fps` padrão 4, `loop` padrão `true`; animação sem loop congela no último frame; anim inexistente cai em `idle`.

```json
{
  "image": "barky.png",
  "cell": [28, 36],
  "origin": [14, 35],
  "anims": {
    "stand_down": { "row": 0, "frames": 1 },
    "walk_down": { "row": 0, "frames": 4, "fps": 6 },
    "stand_up": { "row": 1, "frames": 1 },
    "walk_up": { "row": 1, "frames": 4, "fps": 6 },
    "stand_left": { "row": 2, "frames": 1 },
    "walk_left": { "row": 2, "frames": 2, "fps": 4 },
    "stand_right": { "row": 3, "frames": 1 },
    "walk_right": { "row": 3, "frames": 2, "fps": 4 },
    "drop": { "row": 4, "frames": 3, "fps": 6, "loop": false }
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

- Orelhas idênticas em todos os frames de caminhada.
- No frame de queda o pivô continua em [14,35] (o código move a posição).

## 10. Checklist de aceite

- [ ] O PNG tem exatamente 112×180 px
- [ ] Sem anti-aliasing e sem cores fora da paleta da seção 4
- [ ] Fundo transparente (magenta removido)
- [ ] JSON válido ao lado do PNG, com os nomes de animação da seção 3
- [ ] Silhueta em preto reconhecível pelas orelhas.
- [ ] Alfinete e zíper visíveis de costas.
- [ ] Registrado em `assets/PLACEHOLDERS.md` se não for a arte final
