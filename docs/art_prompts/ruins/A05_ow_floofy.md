# A5 · `assets/sprites/ow/floofy.png` — Floofy — overworld

> Ficha autocontida: tudo o que é preciso para gerar e entregar esta imagem. Índice e regras gerais em [README.md](README.md).

| Campo | Valor |
| :--- | :--- |
| Arquivo final | `assets/sprites/ow/floofy.png` |
| JSON | `assets/sprites/ow/floofy.json` |
| Prioridade | P0 — Ruins |
| Onde aparece | R2 (tutorial falso dos "free hug pellets"), R13 (saída). |
| O código já lê? | Não. Formato pronto para o loader. |

## Envio (resumo)

Conversa nova no gerador → enviar **5 anexos**: `A05_ow_floofy.md` + `templates/A05_ow_floofy.png` + `cards/A05_ow_floofy.png` + *Undertale — Flowey overworld sprites* + *Foto/ilustração real: goat mascot costume head* → colar o prompt da seção 7. Detalhes de cada anexo na seção 6.

## 1. Contexto

Floofy, a flor sem vergonha, paródia do Flowey (e do Asriel). Flor dourada usando a **cabeça de fursuit do Príncipe Azzy**. Sem alma, **sem vergonha**. Fofa e falsa no começo; depois debochada e sem limites. Fala: `Howdy! I'm FLOOFY! Floofy the Flower!`

**Visual (LORE §10.10):** flor amarela de pétalas grandes; o "rosto" é uma cabecinha de fursuit de cabra com olhos de plástico e **um zíper descendo pelo meio da cara**. No modo sinistro o zíper abre e mostra **só escuridão e dois olhos brancos** (nunca nada além disso).

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
- **Célula (um frame):** 24×24 px.
- **Grade:** 3 coluna(s) × 4 linha(s).
- **Tamanho total do PNG:** **72×96 px** (1× nativo; o jogo desenha a 2×).
- **Pivô (`origin`):** [12, 23] dentro de cada célula.
- Personagem ~20×20.
- Pivô = onde o caule entra no chão.
- **Do gerador de imagem:** peça em alta resolução com fundo `#FF00FF`; o arquivo entregue é o pequeno, já reduzido e recortado (ver "Do gerador ao arquivo final").

**Montagem do sheet:**

| Linha | Animação | Frames |
| :---: | :--- | :--- |
| 0 | `idle` | 2: balançando 1 px para os lados |
| 1 | `talk` | 2: **sem boca** (quem fala é o que está dentro da fantasia): a cabeça balança 1 px e o puxador do zíper chacoalha |
| 2 | `sinister` | 2: zíper aberto, escuridão + dois olhos brancos (frame 2: olhos 1 px menores) |
| 3 | `sink` (sem loop) | 3: afundando no chão (inteiro → metade → só pétalas) |

## 4. Paleta

| Uso | Hex |
| :--- | :--- |
| Contorno | `#2A1A08` |
| Pétalas / sombra | `#FFD23F` / `#E6A800` |
| Caule e folhas | `#3FA34D` / `#2C7A38` |
| Cabeça de fursuit | `#F4F0EA` / `#CFC3DD` |
| Olhos de plástico | `#111111 + brilho #FFFFFF` |
| Zíper | `#9A9AAA` |
| Interior aberto | `#000000 + olhos #FFFFFF` |

## 5. Referência atual

Nenhuma. Silhueta do Flowey com a cabeça de mascote no lugar do rosto.

## 6. Pacote de envio (até 5 anexos)

Numa conversa nova, anexe **nesta ordem** e cole o prompt da seção 7:

| # | Anexo | Onde está | Para quê |
| :---: | :--- | :--- | :--- |
| md | Esta ficha | [`A05_ow_floofy.md`](A05_ow_floofy.md) | especificação completa |
| 1 | Template da grade/canvas | [`templates/A05_ow_floofy.png`](templates/A05_ow_floofy.png) | layout exato: proporção, linhas, colunas, pivô |
| 2 | Cartão de estilo | [`cards/A05_ow_floofy.png`](cards/A05_ow_floofy.png) | densidade de pixel (âncora), paleta, tamanho e regras numa imagem |
| 3 | Undertale — Flowey overworld sprites | The Spriters Resource → Undertale (só referência: salve em `assets/_ref/`, git-ignored) | tamanho, caule e animação de afundar |
| 4 | Foto/ilustração real: goat mascot costume head | busca de imagens: "goat mascot costume head" | cabeça de mascote |

**Alternativas** (troque uma imagem acima, ou use como 5ª se o gerador não contar o `.md`):

- Foto/ilustração real: zipper close up — busca de imagens: "zipper close up" — dentes do zíper

Referências são **só entrada**: material do Undertale ou de terceiros fica em `assets/_ref/` (git-ignored) e nunca é commitado nem copiado pixel a pixel.

## 7. Prompt (autocontido)

Cole inteiro. Ele repete as regras de estilo, a paleta e o formato, e explica cada anexo. Se não enviar alguma imagem, apague a linha dela em "Attached".

```text
You are drawing game art for nUndertale, a parody of the game Undertale. Rules: true pixel art in Undertale's style; every art pixel is one uniform square block; 1-pixel dark outline; flat colors with at most one shadow tone per color; no anti-aliasing, gradients, blur, painted texture or semi-transparent edges, including where the drawing meets the background. Match the pixel density of the anchor sprite on the style card: a human-sized overworld character is about 20x30 art pixels. Render the image large, but keep that number of art pixels. Use only the hex colors listed in this prompt. Adult proportions, nothing explicit, no real people, no brand logos, no text except exact words given here, no watermark or signature.

Attached: the spec file A05_ow_floofy.md (full details) and these images, in order:
- image 1: layout template: copy its canvas proportions, rows, columns and cells exactly (one drawing per gray cell, white cells empty, feet or base on the red line); never draw its grid, gray cells, red marks or labels
- image 2: style card: match its pixel density (anchor sprite), palette, size and rules; never draw the card itself; the frog on it is only a pixel-density example, do not draw a frog or copy its features
- image 3: reference for size, stem and sinking animation only
- image 4: reference for mascot head only
Draw a new image; never copy the reference images.

Pixel art sprite in the style of Undertale and Deltarune overworld sprites, 3/4 top-down RPG view, crisp 1-pixel dark outline, flat cel shading with one shadow tone per color, very limited palette, no anti-aliasing, no gradients, no blur, chunky readable pixels, adult proportions, isolated on a solid flat #FF00FF magenta background, no watermark. Small sprite sheet of a golden yellow flower with big rounded petals and a green stem with two leaves, whose face is a tiny cute white baby-goat mascot costume head with shiny black plastic eyes, a silver zipper running down the middle of the face from the top of the head to the chin, with the pull tab at the chin, and no mouth. Strict grid of 4 rows, equal 24x24 cells, stem base at the bottom center: row 1 idle gently swaying (2 frames), row 2 talking (2 frames, no mouth: the voice comes from whatever is inside the costume, so talking is shown only by the head bobbing 1 pixel and the zipper pull tab swinging), row 3 sinister with the face zipper pulled open revealing only pitch-black darkness and two small glowing white dot eyes (2 frames), row 4 sinking into the ground (full, half, only petals visible). About 20x20 pixels of character per frame. No text.

Use only these colors: #2A1A08 (Contorno), #FFD23F (Pétalas), #E6A800 (sombra), #3FA34D (Caule e folhas), #2C7A38 (Caule e folhas), #F4F0EA (Cabeça de fursuit), #CFC3DD (Cabeça de fursuit), #111111 (Olhos de plástico), #9A9AAA (Zíper), #000000 (Interior aberto).
Output: one image with the whole sheet laid out exactly like image 1 (3 columns x 4 rows of equal cells), on a flat, exact #FF00FF magenta background with no noise.
Avoid: blurry, anti-aliased, soft shading, smooth gradient, 3D render, realistic, photo, painterly, noisy texture, random dithering, jpeg artifacts, watermark, signature, brand logo, real person, real face, child proportions, baby face, nsfw, suggestive, cropped, cut off, inconsistent sizes between frames.
```

## 8. JSON

Salvar como `assets/sprites/ow/floofy.json`. `frames` padrão 1, `fps` padrão 4, `loop` padrão `true`; animação sem loop congela no último frame; anim inexistente cai em `idle`.

```json
{
  "image": "floofy.png",
  "cell": [24, 24],
  "origin": [12, 23],
  "anims": {
    "idle": { "row": 0, "frames": 2, "fps": 2 },
    "talk": { "row": 1, "frames": 2, "fps": 4 },
    "sinister": { "row": 2, "frames": 2, "fps": 3 },
    "sink": { "row": 3, "frames": 3, "fps": 6, "loop": false }
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

- No modo sinistro: o interior é **preto puro** com dois pontos brancos — nada mais.

## 10. Checklist de aceite

- [ ] O PNG tem exatamente 72×96 px
- [ ] Sem anti-aliasing e sem cores fora da paleta da seção 4
- [ ] Fundo transparente (magenta removido)
- [ ] JSON válido ao lado do PNG, com os nomes de animação da seção 3
- [ ] Lê como flor + cabeça de mascote mesmo em 20×20.
- [ ] Regra de conteúdo do modo sinistro respeitada.
- [ ] Registrado em `assets/PLACEHOLDERS.md` se não for a arte final
