# E2 · `assets/ui/souls.png` — Almas (SOUL) do jogador

> Ficha autocontida: tudo o que é preciso para gerar e entregar esta imagem. Índice e regras gerais em [README.md](README.md).

| Campo | Valor |
| :--- | :--- |
| Arquivo final | `assets/ui/souls.png` |
| JSON | `assets/ui/souls.json` |
| Prioridade | P0 — Ruins · **núcleo da vertical slice** |
| Onde aparece | Toda batalha + menus. |
| O código já lê? | Não (hoje: `drawSoul` com primitivas). |

## Envio (resumo)

Conversa nova no gerador → enviar **5 anexos**: `E02_ui_souls.md` + `templates/E02_ui_souls.png` + `cards/E02_ui_souls.png` + *Undertale — SOUL heart sprites* + *Foto/ilustração real: plush heart with ears* → colar o prompt da seção 7. Detalhes de cada anexo na seção 6.

## 1. Contexto

A alma que o jogador controla na caixa de batalha. Cores dos modos clássicos (de `Constants.hpp`) + duas do nUndertale: **pelúcia rosa com orelhinhas** (modo FURRY) e **sapo** (a alma gamer do Pepe). O hitbox no código é 8×8 e não muda.

## 2. Regras que valem para esta imagem

- **Escala:** arte em **1× nativo** (tela cheia = 320×240). O jogo desenha a **2×** com filtro *point* num canvas virtual de 640×480. Todos os tamanhos desta ficha são 1×.
- **Pixel:** sem anti-aliasing, sem gradiente suave, sem blur, sem sub-pixel.
- **Sprite sheet:** grade fixa, células do mesmo tamanho, sem espaçamento nem margem. **Uma linha por animação**, frames da esquerda para a direita **começando na coluna 0**. O loader não sabe começar em outra coluna, então **estados/variantes diferentes vão em linhas diferentes**. Célula vazia = transparente.
- **Pivô (`origin`):** o mesmo ponto (pé / base central) em todas as células.
- **Conteúdo:** nada explícito; proporções **adultas**; nenhum rosto real; nenhuma marca real (nada de logo Doritos / Mountain Dew; use "CITRUS BLAST", chips sem marca); todo texto dentro da arte em **inglês**.
- **Undertale:** pode servir de referência de tamanho/estilo, mas nada copiado vai para o repositório.
- Elemento de interface/projétil: formas sólidas, legíveis em 2×, sem anti-aliasing.
- Projéteis são brancos `#FFFFFF` (salvo indicação), com o núcleo sólido centralizado na célula (o hitbox será um retângulo menor no centro).

## 3. Formato de output esperado

- **Tipo:** PNG RGBA 32-bit, fundo transparente.
- **Célula (um frame):** 12×12 px.
- **Grade:** 1 coluna(s) × 6 linha(s).
- **Tamanho total do PNG:** **12×72 px** (1× nativo; o jogo desenha a 2×).
- **Pivô (`origin`):** [6, 6] dentro de cada célula.
- Coração 8×8 centralizado na célula 12×12; orelhas/olhos podem ocupar a folga.
- Uma alma por linha.
- Linhas futuras (P2): rachada, estilhaços, sequência FURRY OVER.
- **Do gerador de imagem:** peça em alta resolução com fundo `#FF00FF`; o arquivo entregue é o pequeno, já reduzido e recortado (ver "Do gerador ao arquivo final").

**Montagem do sheet:**

| Linha | Animação | Alma | Cor |
| :---: | :--- | :--- | :--- |
| 0 | `red` | normal | `#FF0000` |
| 1 | `blue` | gravidade | `#0026FF` |
| 2 | `green` | escudo | `#00C000` |
| 3 | `yellow` | tiro | `#FFFF00` |
| 4 | `plush` | pelúcia rosa com orelhinhas | `#FFB3E6` + costura `#FF77C8` |
| 5 | `frog` | sapo, olhinhos no topo | `#6FC418` + olhos `#FFFFFF`/`#000000` |

## 4. Paleta

| Uso | Hex |
| :--- | :--- |
| Vermelha | `#FF0000` |
| Azul | `#0026FF` |
| Verde | `#00C000` |
| Amarela | `#FFFF00` |
| Pelúcia / costura | `#FFB3E6` / `#FF77C8` |
| Sapo | `#6FC418` |

## 5. Referência atual

Nenhuma imagem; desenho atual em código.

## 6. Pacote de envio (até 5 anexos)

Numa conversa nova, anexe **nesta ordem** e cole o prompt da seção 7:

| # | Anexo | Onde está | Para quê |
| :---: | :--- | :--- | :--- |
| md | Esta ficha | [`E02_ui_souls.md`](E02_ui_souls.md) | especificação completa |
| 1 | Template da grade/canvas | [`templates/E02_ui_souls.png`](templates/E02_ui_souls.png) | layout exato: proporção, linhas, colunas, pivô |
| 2 | Cartão de estilo | [`cards/E02_ui_souls.png`](cards/E02_ui_souls.png) | densidade de pixel (âncora), paleta, tamanho e regras numa imagem |
| 3 | Undertale — SOUL heart sprites | The Spriters Resource → Undertale (só referência: salve em `assets/_ref/`, git-ignored) | formato exato do coração 8x8 |
| 4 | Foto/ilustração real: plush heart with ears | busca de imagens: "plush heart with ears" | alma pelúcia |

Referências são **só entrada**: material do Undertale ou de terceiros fica em `assets/_ref/` (git-ignored) e nunca é commitado nem copiado pixel a pixel.

## 7. Prompt (autocontido)

Cole inteiro. Ele repete as regras de estilo, a paleta e o formato, e explica cada anexo. Se não enviar alguma imagem, apague a linha dela em "Attached".

```text
You are drawing game art for nUndertale, a parody of the game Undertale. Rules: true pixel art in Undertale's style; every art pixel is one uniform square block; 1-pixel dark outline; flat colors with at most one shadow tone per color; no anti-aliasing, gradients, blur, painted texture or semi-transparent edges, including where the drawing meets the background. Match the pixel density of the anchor sprite on the style card: a human-sized overworld character is about 20x30 art pixels. Render the image large, but keep that number of art pixels. Use only the hex colors listed in this prompt. Adult proportions, nothing explicit, no real people, no brand logos, no text except exact words given here, no watermark or signature.

Attached: the spec file E02_ui_souls.md (full details) and these images, in order:
- image 1: layout template: copy its canvas proportions, rows, columns and cells exactly (one drawing per gray cell, white cells empty, each drawing centered in its cell); never draw its grid, gray cells, red marks or labels
- image 2: style card: match its pixel density (anchor sprite), palette, size and rules; never draw the card itself; the frog on it is only a pixel-density example, do not draw a frog or copy its features
- image 3: reference for exact 8x8 heart shape only
- image 4: reference for plush heart only
Draw a new image; never copy the reference images.

Tiny pixel art game element in the style of Undertale, crisp hard pixels, no anti-aliasing, no gradients, flat colors, high contrast, isolated on a solid flat #FF00FF magenta background, no watermark. Six tiny pixel heart souls stacked vertically, each an 8x8 pixel heart centered in an equal 12x12 cell: solid red #FF0000, solid blue #0026FF, solid green #00C000, solid yellow #FFFF00, a pink plush heart #FFB3E6 with two tiny fluffy ears and a stitched seam #FF77C8, and a green frog heart #6FC418 with two small bulging eyes on top.

Use only these colors: #FF0000 (Vermelha), #0026FF (Azul), #00C000 (Verde), #FFFF00 (Amarela), #FFB3E6 (Pelúcia), #FF77C8 (costura), #6FC418 (Sapo).
Output: one image with the whole sheet laid out exactly like image 1 (1 columns x 6 rows of equal cells), on a flat, exact #FF00FF magenta background with no noise.
Avoid: blurry, anti-aliased, soft shading, smooth gradient, 3D render, realistic, photo, painterly, noisy texture, random dithering, jpeg artifacts, watermark, signature, brand logo, real person, real face, child proportions, baby face, nsfw, suggestive, cropped, cut off, inconsistent sizes between frames.
```

## 8. JSON

Salvar como `assets/ui/souls.json`. `frames` padrão 1, `fps` padrão 4, `loop` padrão `true`; animação sem loop congela no último frame; anim inexistente cai em `idle`.

```json
{
  "image": "souls.png",
  "cell": [12, 12],
  "origin": [6, 6],
  "anims": {
    "red": { "row": 0, "frames": 1 },
    "blue": { "row": 1, "frames": 1 },
    "green": { "row": 2, "frames": 1 },
    "yellow": { "row": 3, "frames": 1 },
    "plush": { "row": 4, "frames": 1 },
    "frog": { "row": 5, "frames": 1 }
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

- Os 4 corações clássicos devem ter **exatamente** o mesmo formato.

## 10. Checklist de aceite

- [ ] O PNG tem exatamente 12×72 px
- [ ] Sem anti-aliasing e sem cores fora da paleta da seção 4
- [ ] Fundo transparente (magenta removido)
- [ ] JSON válido ao lado do PNG, com os nomes de animação da seção 3
- [ ] Legível a 2× (16×16 na tela).
- [ ] Registrado em `assets/PLACEHOLDERS.md` se não for a arte final
