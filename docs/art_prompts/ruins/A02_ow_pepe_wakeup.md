# A2 · `assets/sprites/ow/pepe_wakeup.png` — Pepe deitado / levantando (R1)

> Ficha autocontida: tudo o que é preciso para gerar e entregar esta imagem. Índice e regras gerais em [README.md](README.md).

| Campo | Valor |
| :--- | :--- |
| Arquivo final | `assets/sprites/ow/pepe_wakeup.png` |
| JSON | `assets/sprites/ow/pepe_wakeup.json` |
| Prioridade | P0 — Ruins |
| Onde aparece | R1 — The Fall Room. |
| O código já lê? | Não. **Nome sugerido** — ainda não referenciado; o cutscene runner (CORE-011) pode usar. |

## Envio (resumo)

Conversa nova no gerador → enviar **5 anexos**: `A02_ow_pepe_wakeup.md` + `templates/A02_ow_pepe_wakeup.png` + `cards/A02_ow_pepe_wakeup.png` + `assets/sprites/ow/pepe_frog.png` + *Undertale — Frisk deitado no canteiro de flores (início do jogo)* → colar o prompt da seção 7. Detalhes de cada anexo na seção 6.

## 1. Contexto

Abertura do jogo: Pepe acorda numa cama de embalagens de salgadinho sob um feixe de luz. Pensa: `* (Ugh... where's my wireless mouse? And why does it smell like body spray and wet dog?)`. Mesmo personagem da ficha A1 e do sprite-âncora: o sapo do meme (cabeça larga com domos dos olhos, pálpebras pesadas, dois lábios marrom-avermelhados), **camiseta azul** com manchas de nacho, short azul claro, chinelo.

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
- **Célula (um frame):** 32×32 px.
- **Grade:** 3 coluna(s) × 2 linha(s).
- **Tamanho total do PNG:** **96×64 px** (1× nativo; o jogo desenha a 2×).
- **Pivô (`origin`):** [16, 31] dentro de cada célula.
- Deitado ocupa ~30×12 na horizontal; por isso a célula é mais larga que a do A1.
- O último frame de `get_up` deve ser idêntico ao `stand_down` do A1.
- **Do gerador de imagem:** peça em alta resolução com fundo `#FF00FF`; o arquivo entregue é o pequeno, já reduzido e recortado (ver "Do gerador ao arquivo final").

**Montagem do sheet:**

| Linha | Animação | Frames |
| :---: | :--- | :--- |
| 0 | `lying` | 1: deitado de barriga para cima, olhos fechados, braços abertos |
| 1 | `get_up` (sem loop) | 3: sentado esfregando o olho → ajoelhado se apoiando → de pé de frente |

## 4. Paleta

| Uso | Hex |
| :--- | :--- |
| Contorno | `#3D120E` |
| Pele / sombra | `#5FA33A` / `#3F7A2A` |
| Linha da olheira | `#2E5E22` |
| Olho / pupila | `#FFFFFF` / `#141414` |
| Lábios | `#C8553D` / `#A8432E` |
| Camiseta / sombra | `#2F62D6` / `#1F449E` |
| Nacho | `#F0A03A` |
| Short | `#67A4E0` / `#4A80BC` |
| Chinelo | `#2B2B2B` |

## 5. Referência atual

Nenhuma. Use o sprite-âncora e o sheet final da ficha A1 como referência de personagem.

## 6. Pacote de envio (até 5 anexos)

Numa conversa nova, anexe **nesta ordem** e cole o prompt da seção 7:

| # | Anexo | Onde está | Para quê |
| :---: | :--- | :--- | :--- |
| md | Esta ficha | [`A02_ow_pepe_wakeup.md`](A02_ow_pepe_wakeup.md) | especificação completa |
| 1 | Template da grade/canvas | [`templates/A02_ow_pepe_wakeup.png`](templates/A02_ow_pepe_wakeup.png) | layout exato: proporção, linhas, colunas, pivô |
| 2 | Cartão de estilo | [`cards/A02_ow_pepe_wakeup.png`](cards/A02_ow_pepe_wakeup.png) | densidade de pixel (âncora), paleta, tamanho e regras numa imagem |
| 3 | Saída aprovada da ficha A1 | `assets/sprites/ow/pepe_frog.png` (depois de aprovada) | mesmo personagem / mesmo design |
| 4 | Undertale — Frisk deitado no canteiro de flores (início do jogo) | The Spriters Resource → Undertale (só referência: salve em `assets/_ref/`, git-ignored) | pose deitado e levantando |

Referências são **só entrada**: material do Undertale ou de terceiros fica em `assets/_ref/` (git-ignored) e nunca é commitado nem copiado pixel a pixel.

## 7. Prompt (autocontido)

Cole inteiro. Ele repete as regras de estilo, a paleta e o formato, e explica cada anexo. Se não enviar alguma imagem, apague a linha dela em "Attached".

```text
You are drawing game art for nUndertale, a parody of the game Undertale. Rules: true pixel art in Undertale's style; every art pixel is one uniform square block; 1-pixel dark outline; flat colors with at most one shadow tone per color; no anti-aliasing, gradients, blur, painted texture or semi-transparent edges, including where the drawing meets the background. Match the pixel density of the anchor sprite on the style card: a human-sized overworld character is about 20x30 art pixels. Render the image large, but keep that number of art pixels. Use only the hex colors listed in this prompt. Adult proportions, nothing explicit, no real people, no brand logos, no text except exact words given here, no watermark or signature.

Attached: the spec file A02_ow_pepe_wakeup.md (full details) and these images, in order:
- image 1: layout template: copy its canvas proportions, rows, columns and cells exactly (one drawing per gray cell, white cells empty, feet or base on the red line); never draw its grid, gray cells, red marks or labels
- image 2: style card: match its pixel density (anchor sprite), palette, size and rules; never draw the card itself
- image 3: the approved design of the same character: keep its design exactly
- image 4: reference for lying and getting-up pose only
Draw a new image; never copy the reference images.

Pixel art sprite in the style of Undertale and Deltarune overworld sprites, 3/4 top-down RPG view, crisp 1-pixel dark outline, flat cel shading with one shadow tone per color, very limited palette, no anti-aliasing, no gradients, no blur, chunky readable pixels, adult proportions, isolated on a solid flat #FF00FF magenta background, no watermark. Small sprite sheet of the same cartoon frog man as the anchor sprite, in the classic sad-frog comic look (wide lumpy head with two eye domes on top, heavy droopy eyelids, wide two-part reddish-brown lips, medium frog-green skin #5FA33A, plain blue T-shirt #2F62D6 with orange nacho stains, light blue shorts, black flip-flops, short pudgy adult body). Row 1: lying flat on his back on the ground, eyes closed, arms spread, seen from a 3/4 top-down view. Row 2, three frames: sitting up rubbing one eye, kneeling and pushing himself up, standing facing the viewer slouched. Same scale as a 20x30 pixel character, each frame in an equal 32x32 cell, feet aligned to the bottom. Dark brown outline #3D120E. No text.

Use only these colors: #3D120E (Contorno), #5FA33A (Pele), #3F7A2A (sombra), #2E5E22 (Linha da olheira), #FFFFFF (Olho), #141414 (pupila), #C8553D (Lábios), #A8432E (Lábios), #2F62D6 (Camiseta), #1F449E (sombra), #F0A03A (Nacho), #67A4E0 (Short), #4A80BC (Short), #2B2B2B (Chinelo).
Output: one image with the whole sheet laid out exactly like image 1 (3 columns x 2 rows of equal cells), on a flat, exact #FF00FF magenta background with no noise.
Avoid: blurry, anti-aliased, soft shading, smooth gradient, 3D render, realistic, photo, painterly, noisy texture, random dithering, jpeg artifacts, watermark, signature, brand logo, real person, real face, child proportions, baby face, nsfw, suggestive, cropped, cut off, inconsistent sizes between frames.
```

## 8. JSON

Salvar como `assets/sprites/ow/pepe_wakeup.json`. `frames` padrão 1, `fps` padrão 4, `loop` padrão `true`; animação sem loop congela no último frame; anim inexistente cai em `idle`.

```json
{
  "image": "pepe_wakeup.png",
  "cell": [32, 32],
  "origin": [16, 31],
  "anims": {
    "lying": { "row": 0, "frames": 1 },
    "get_up": { "row": 1, "frames": 3, "fps": 4, "loop": false }
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

- Reaproveite a cabeça/cores do A1 para manter o mesmo personagem.
- Alinhe o último frame pixel a pixel com o `stand_down` do A1.

## 10. Checklist de aceite

- [ ] O PNG tem exatamente 96×64 px
- [ ] Sem anti-aliasing e sem cores fora da paleta da seção 4
- [ ] Fundo transparente (magenta removido)
- [ ] JSON válido ao lado do PNG, com os nomes de animação da seção 3
- [ ] A transição `get_up` → `stand_down` não dá salto visível.
- [ ] Registrado em `assets/PLACEHOLDERS.md` se não for a arte final
