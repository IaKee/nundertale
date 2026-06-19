# A3 · `assets/sprites/ow/furiell.png` — Furiell — overworld

> Ficha autocontida: tudo o que é preciso para gerar e entregar esta imagem. Índice e regras gerais em [README.md](README.md).

| Campo | Valor |
| :--- | :--- |
| Arquivo final | `assets/sprites/ow/furiell.png` |
| JSON | `assets/sprites/ow/furiell.json` |
| Prioridade | P0 — Ruins |
| Onde aparece | R2 (chegada), R3–R12 (guia o Pepe; casa dela em R11). |
| O código já lê? | Não (ainda desenhada com primitivas / ausente). Formato pronto para o loader. |

## Envio (resumo)

Conversa nova no gerador → enviar **5 anexos**: `A03_ow_furiell.md` + `templates/A03_ow_furiell.png` + `cards/A03_ow_furiell.png` + *Undertale — Toriel overworld sprite sheet* + *Foto/ilustração real: white goat mascot costume head* → colar o prompt da seção 7. Detalhes de cada anexo na seção 6.

## 1. Contexto

Furiell, Guardiã das Ruins, paródia da Toriel. Ex-rainha; vive nas Ruins para "converter com amor" os gamers que caem. Campeã do *love-bombing*: mãe sufocante, gentil, docemente passivo-agressiva, chama todo mundo de "my child". Faz Pop-Tarts de glitter.

**Visual (LORE §10.2):** fursuit de cabra **branca e fofa**, óculos meia-lua, **avental com estampa de patinhas**, **zíper visível nas costas** (ela jura que não existe). Nunca é sexual com o Pepe; o "nojento" dela é o que ela coleciona.

## 2. Regras que valem para esta imagem

- **Escala:** arte em **1× nativo** (tela cheia = 320×240). O jogo desenha a **2×** com filtro *point* num canvas virtual de 640×480. Todos os tamanhos desta ficha são 1×.
- **Pixel:** sem anti-aliasing, sem gradiente suave, sem blur, sem sub-pixel.
- **Sprite sheet:** grade fixa, células do mesmo tamanho, sem espaçamento nem margem. **Uma linha por animação**, frames da esquerda para a direita **começando na coluna 0**. O loader não sabe começar em outra coluna, então **estados/variantes diferentes vão em linhas diferentes**. Célula vazia = transparente.
- **Pivô (`origin`):** o mesmo ponto (pé / base central) em todas as células.
- **Conteúdo:** nada explícito; proporções **adultas**; nenhum rosto real; nenhuma marca real (nada de logo Doritos / Mountain Dew; use "CITRUS BLAST", chips sem marca); todo texto dentro da arte em **inglês**.
- **Undertale:** pode servir de referência de tamanho/estilo, mas nada copiado vai para o repositório.
- Overworld é **colorido**, com paleta limitada (Ruins: roxo + verde neon).
- Vista 3/4 de cima, igual aos sprites de overworld do Undertale.
- Contorno de 1 px **quase preto `#141018`** fechando a silhueta inteira (nunca roxo escuro: o chão das Ruins é roxo e engole o contorno). O Pepe mantém o contorno próprio `#3D120E`. Uma sombra por cor; sem anti-aliasing.

## 3. Formato de output esperado

- **Tipo:** PNG RGBA 32-bit, fundo transparente.
- **Célula (um frame):** 32×44 px.
- **Grade:** 4 coluna(s) × 5 linha(s).
- **Tamanho total do PNG:** **128×220 px** (1× nativo; o jogo desenha a 2×).
- **Pivô (`origin`):** [16, 43] dentro de cada célula.
- Personagem ~26×40 dentro da célula.
- O zíper prateado tem que aparecer na linha de costas (linha 1).
- **Do gerador de imagem:** peça em alta resolução com fundo `#FF00FF`; o arquivo entregue é o pequeno, já reduzido e recortado (ver "Do gerador ao arquivo final").

**Montagem do sheet:**

| Linha | Animação | Col 0 | Col 1 | Col 2 | Col 3 |
| :---: | :--- | :--- | :--- | :--- | :--- |
| 0 | `walk_down` / `stand_down` | parado, de frente | passo pé esquerdo | parado (cópia da col 0) | passo pé direito |
| 1 | `walk_up` / `stand_up` | parado, de costas | passo esquerdo | parado (cópia) | passo direito |
| 2 | `walk_left` / `stand_left` | parado, perfil esquerdo | passo | *(vazio)* | *(vazio)* |
| 3 | `walk_right` / `stand_right` | parado, perfil direito | passo | *(vazio)* | *(vazio)* |
| 4 | `talk_down` | boca fechada | boca aberta | *(vazio)* | *(vazio)* |

## 4. Paleta

| Uso | Hex |
| :--- | :--- |
| Contorno | `#2A1640` |
| Pelo branco / sombra lilás | `#F4F0EA` / `#CFC3DD` |
| Chifres | `#E6D8B8` |
| Avental / estampa de patinhas | `#C9A0DC` / `#5E3A87` |
| Vestido | `#5E3A87` |
| Óculos (aro dourado) | `#E6B422` |
| Zíper | `#9A9AAA` |
| Olhos | `#3B1F5C` |

## 5. Referência atual

Nenhuma no repositório. Silhueta da Toriel (alta, ombros largos, vestido longo), mas claramente uma **fursuit**: cabeçorra fofa, dobra de tecido no pescoço.

## 6. Pacote de envio (até 5 anexos)

Numa conversa nova, anexe **nesta ordem** e cole o prompt da seção 7:

| # | Anexo | Onde está | Para quê |
| :---: | :--- | :--- | :--- |
| md | Esta ficha | [`A03_ow_furiell.md`](A03_ow_furiell.md) | especificação completa |
| 1 | Template da grade/canvas | [`templates/A03_ow_furiell.png`](templates/A03_ow_furiell.png) | layout exato: proporção, linhas, colunas, pivô |
| 2 | Cartão de estilo | [`cards/A03_ow_furiell.png`](cards/A03_ow_furiell.png) | densidade de pixel (âncora), paleta, tamanho e regras numa imagem |
| 3 | Undertale — Toriel overworld sprite sheet | The Spriters Resource → Undertale (só referência: salve em `assets/_ref/`, git-ignored) | silhueta, altura e layout |
| 4 | Foto/ilustração real: white goat mascot costume head | busca de imagens: "white goat mascot costume head" | cabeça de fursuit de cabra |

**Alternativas** (troque uma imagem acima, ou use como 5ª se o gerador não contar o `.md`):

- Foto/ilustração real: half-moon reading glasses — busca de imagens: "half-moon reading glasses" — óculos meia-lua
- Foto/ilustração real: paw print apron — busca de imagens: "paw print apron" — avental com patinhas

Referências são **só entrada**: material do Undertale ou de terceiros fica em `assets/_ref/` (git-ignored) e nunca é commitado nem copiado pixel a pixel.

## 7. Prompt (autocontido)

Cole inteiro. Ele repete as regras de estilo, a paleta e o formato, e explica cada anexo. Se não enviar alguma imagem, apague a linha dela em "Attached".

```text
You are drawing game art for nUndertale, a parody of the game Undertale. Rules: true pixel art in Undertale's style; every art pixel is one uniform square block; 1-pixel dark outline; flat colors with at most one shadow tone per color; no anti-aliasing, gradients, blur, painted texture or semi-transparent edges, including where the drawing meets the background. Match the pixel density of the anchor sprite on the style card: a human-sized overworld character is about 20x30 art pixels. Render the image large, but keep that number of art pixels. Use only the hex colors listed in this prompt. Adult proportions, nothing explicit, no real people, no brand logos, no text except exact words given here, no watermark or signature.

Attached: the spec file A03_ow_furiell.md (full details) and these images, in order:
- image 1: layout template: copy its canvas proportions, rows, columns and cells exactly (one drawing per gray cell, white cells empty, feet or base on the red line); never draw its grid, gray cells, red marks or labels
- image 2: style card: match its pixel density (anchor sprite), palette, size and rules; never draw the card itself; the frog on it is only a pixel-density example, do not draw a frog or copy its features
- image 3: reference for silhouette, height and layout only
- image 4: reference for goat costume head only
Draw a new image; never copy the reference images.

Pixel art sprite in the style of Undertale and Deltarune overworld sprites, 3/4 top-down RPG view, crisp 1-pixel near-black outline (never dark purple: the Ruins floor is purple), flat cel shading with one shadow tone per color, very limited palette, no anti-aliasing, no gradients, no blur, chunky readable pixels, adult proportions, isolated on a solid flat #FF00FF magenta background, no watermark. Character sprite sheet of a tall motherly adult woman wearing a fluffy white goat fursuit (oversized soft costume head with small cream horns and floppy ears, a visible fabric fold at the neck), small gold half-moon reading glasses, a long purple dress under a lilac apron with a dark purple paw-print pattern, kind smile, gentle posture with paws clasped. Seen from behind, a silver costume zipper runs clearly down the middle of her back. Strict grid of 5 rows, all frames the same size, feet on the same baseline: row 1 facing down (idle, left step, idle, right step), row 2 facing up with the zipper visible (idle, left step, idle, right step), row 3 facing left (idle, step), row 4 facing right (idle, step), row 5 facing down talking (mouth closed, mouth open). White fur #F4F0EA with lilac shadows, apron #C9A0DC, dark purple outline #2A1640. Adult proportions, about 26x40 pixels per frame. No text.

Use only these colors: #2A1640 (Contorno), #F4F0EA (Pelo branco), #CFC3DD (sombra lilás), #E6D8B8 (Chifres), #C9A0DC (Avental), #5E3A87 (estampa de patinhas), #5E3A87 (Vestido), #E6B422 (Óculos), #9A9AAA (Zíper), #3B1F5C (Olhos).
Output: one image with the whole sheet laid out exactly like image 1 (4 columns x 5 rows of equal cells), on a flat, exact #FF00FF magenta background with no noise.
Avoid: blurry, anti-aliased, soft shading, smooth gradient, 3D render, realistic, photo, painterly, noisy texture, random dithering, jpeg artifacts, watermark, signature, brand logo, real person, real face, child proportions, baby face, nsfw, suggestive, cropped, cut off, inconsistent sizes between frames.
```

## 8. JSON

Salvar como `assets/sprites/ow/furiell.json`. `frames` padrão 1, `fps` padrão 4, `loop` padrão `true`; animação sem loop congela no último frame; anim inexistente cai em `idle`.

```json
{
  "image": "furiell.png",
  "cell": [32, 44],
  "origin": [16, 43],
  "anims": {
    "stand_down": { "row": 0, "frames": 1 },
    "walk_down": { "row": 0, "frames": 4, "fps": 6 },
    "stand_up": { "row": 1, "frames": 1 },
    "walk_up": { "row": 1, "frames": 4, "fps": 6 },
    "stand_left": { "row": 2, "frames": 1 },
    "walk_left": { "row": 2, "frames": 2, "fps": 4 },
    "stand_right": { "row": 3, "frames": 1 },
    "walk_right": { "row": 3, "frames": 2, "fps": 4 },
    "talk_down": { "row": 4, "frames": 2, "fps": 4 }
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

- Mantenha os chifres e orelhas idênticos entre frames.
- Zíper: 1 px de largura, 3–4 px de dentes visíveis.

## 10. Checklist de aceite

- [ ] O PNG tem exatamente 128×220 px
- [ ] Sem anti-aliasing e sem cores fora da paleta da seção 4
- [ ] Fundo transparente (magenta removido)
- [ ] JSON válido ao lado do PNG, com os nomes de animação da seção 3
- [ ] Lê como fursuit (não como cabra real).
- [ ] Zíper visível de costas.
- [ ] Proporções adultas.
- [ ] Registrado em `assets/PLACEHOLDERS.md` se não for a arte final
