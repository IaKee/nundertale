# B5 · `assets/sprites/battle/froggsona.png` — Froggsona — batalha (inimigo comum, paródia de Froggit)

> Ficha autocontida: tudo o que é preciso para gerar e entregar esta imagem. Índice e regras gerais em [README.md](README.md).

| Campo | Valor |
| :--- | :--- |
| Arquivo final | `assets/sprites/battle/froggsona.png` |
| JSON | `assets/sprites/battle/froggsona.json` |
| Prioridade | P0 — Ruins |
| Onde aparece | R8 (encontro aleatório; pool com Shysona e Modsmal). |
| O código já lê? | Não. Formato pronto para o loader. |

## Envio (resumo)

Conversa nova no gerador → enviar **5 anexos**: `B05_battle_froggsona.md` + `templates/B05_battle_froggsona.png` + `cards/B05_battle_froggsona.png` + *Undertale — Froggit battle sprite* + *Foto/ilustração real: homemade frog costume* → colar o prompt da seção 7. Detalhes de cada anexo na seção 6.

## 1. Contexto

HP 30 · AT 4 · DF 2. Check: `* FROGGSONA - ATK 4 DEF 2` / `* A frog fursona. Every sentence ends in ":3".`
Balões: `ribbit :3` · `*ribbits suggestively*`. Pensamento do Pepe: `* (It looks like you. A little. Don't think about it.)`
ACTs: Compliment (`* You say "nice tail". It doesn't have a tail. It's flattered anyway.`), Threaten, Accept Hug.
**Visual:** sapinho numa fursuit caseira de sapo, olhos de fantasia enormes, boca de gato `:3`, coleira com sininho.

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
- **Célula (um frame):** 40×40 px.
- **Grade:** 2 coluna(s) × 2 linha(s).
- **Tamanho total do PNG:** **80×80 px** (1× nativo; o jogo desenha a 2×).
- **Pivô (`origin`):** [20, 40] dentro de cada célula.
- Grade padrão dos inimigos comuns: linha 0 `idle` (2 frames), linha 1 `hurt` (1 frame).
- Pivô = centro da base.
- **Do gerador de imagem:** peça em alta resolução com fundo `#FF00FF`; o arquivo entregue é o pequeno, já reduzido e recortado (ver "Do gerador ao arquivo final").

**Montagem do sheet:**

| Linha | Animação | Frames |
| :---: | :--- | :--- |
| 0 | `idle` | 2 |
| 1 | `hurt` | 1 |

## 4. Paleta

| Uso | Hex |
| :--- | :--- |
| Traço | `#FFFFFF` |
| Preenchimento interno (opaco) | `#000000` |

## 5. Referência atual

Nenhuma.

## 6. Pacote de envio (até 5 anexos)

Numa conversa nova, anexe **nesta ordem** e cole o prompt da seção 7:

| # | Anexo | Onde está | Para quê |
| :---: | :--- | :--- | :--- |
| md | Esta ficha | [`B05_battle_froggsona.md`](B05_battle_froggsona.md) | especificação completa |
| 1 | Template da grade/canvas | [`templates/B05_battle_froggsona.png`](templates/B05_battle_froggsona.png) | layout exato: proporção, linhas, colunas, pivô |
| 2 | Cartão de estilo | [`cards/B05_battle_froggsona.png`](cards/B05_battle_froggsona.png) | densidade de pixel (âncora), paleta, tamanho e regras numa imagem |
| 3 | Undertale — Froggit battle sprite | The Spriters Resource → Undertale (só referência: salve em `assets/_ref/`, git-ignored) | estilo do traço branco sobre preto |
| 4 | Foto/ilustração real: homemade frog costume | busca de imagens: "homemade frog costume" | fantasia caseira de sapo |

Referências são **só entrada**: material do Undertale ou de terceiros fica em `assets/_ref/` (git-ignored) e nunca é commitado nem copiado pixel a pixel.

## 7. Prompt (autocontido)

Cole inteiro. Ele repete as regras de estilo, a paleta e o formato, e explica cada anexo. Se não enviar alguma imagem, apague a linha dela em "Attached".

```text
You are drawing game art for nUndertale, a parody of the game Undertale. Rules: true pixel art in Undertale's style; every art pixel is one uniform square block; 1-pixel dark outline; flat colors with at most one shadow tone per color; no anti-aliasing, gradients, blur, painted texture or semi-transparent edges, including where the drawing meets the background. Match the pixel density of the anchor sprite on the style card: a human-sized overworld character is about 20x30 art pixels. Render the image large, but keep that number of art pixels. Use only the hex colors listed in this prompt. Adult proportions, nothing explicit, no real people, no brand logos, no text except exact words given here, no watermark or signature.

Attached: the spec file B05_battle_froggsona.md (full details) and these images, in order:
- image 1: layout template: copy its canvas proportions, rows, columns and cells exactly (one drawing per gray cell, white cells empty, feet or base on the red line); never draw its grid, gray cells, red marks or labels
- image 2: style card: match its pixel density (anchor sprite), palette, size and rules; never draw the card itself; the frog on it is only a pixel-density example, do not draw a frog or copy its features
- image 3: reference for white-on-black line style only
- image 4: reference for homemade frog costume only
Draw a new image; never copy the reference images.

Undertale battle sprite, pure white 1-pixel line art on a solid black fill, monochrome (only #FFFFFF and #000000), bold readable silhouette, hand-drawn pixel art, no anti-aliasing, no gray, no gradients, front-facing, centered, isolated on a solid flat #FF00FF magenta background, no watermark. Battle sprite of a small round frog-character in a homemade frog fursuit, huge glossy costume eyes, a cat-like ":3" mouth, a little collar with a bell, squatting pose. Front view, about 40x40 pixels. Row 1: idle (2 frames, small hop squash). Row 2: hurt (eyes as X marks, flattened). Each frame in an equal 40x40 cell. No text.

Use only these colors: #FFFFFF (Traço), #000000 (Preenchimento interno).
Output: one image with the whole sheet laid out exactly like image 1 (2 columns x 2 rows of equal cells), on a flat, exact #FF00FF magenta background with no noise.
Avoid: blurry, anti-aliased, soft shading, smooth gradient, 3D render, realistic, photo, painterly, noisy texture, random dithering, jpeg artifacts, watermark, signature, brand logo, real person, real face, child proportions, baby face, nsfw, suggestive, cropped, cut off, inconsistent sizes between frames.
```

## 8. JSON

Salvar como `assets/sprites/battle/froggsona.json`. `frames` padrão 1, `fps` padrão 4, `loop` padrão `true`; animação sem loop congela no último frame; anim inexistente cai em `idle`.

```json
{
  "image": "froggsona.png",
  "cell": [40, 40],
  "origin": [20, 40],
  "anims": {
    "idle": { "row": 0, "frames": 2, "fps": 2 },
    "hurt": { "row": 1, "frames": 1 }
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

- Preto opaco no interior.
- Frame 2 do idle muda só 1–2 px (balanço), para não "pular".

## 10. Checklist de aceite

- [ ] O PNG tem exatamente 80×80 px
- [ ] Sem anti-aliasing e sem cores fora da paleta da seção 4
- [ ] Fundo transparente (magenta removido)
- [ ] JSON válido ao lado do PNG, com os nomes de animação da seção 3
- [ ] A piada do personagem lê sem texto.
- [ ] Registrado em `assets/PLACEHOLDERS.md` se não for a arte final
