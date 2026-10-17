# E5 · `assets/ui/slash.png` — Golpe (hitmarker de FPS)

> Ficha autocontida: tudo o que é preciso para gerar e entregar esta imagem. Índice e regras gerais em [README.md](README.md).

| Campo | Valor |
| :--- | :--- |
| Arquivo final | `assets/ui/slash.png` |
| JSON | `assets/ui/slash.json` |
| Prioridade | P0 — Ruins |
| Onde aparece | Toda batalha, ao acertar. |
| O código já lê? | Não (hoje: primitiva). |

## Envio (resumo)

Conversa nova no gerador → enviar **5 anexos**: `E05_ui_slash.md` + `templates/E05_ui_slash.png` + `cards/E05_ui_slash.png` + *Undertale — slash effect* + *Foto/ilustração real: fps hitmarker* → colar o prompt da seção 7. Detalhes de cada anexo na seção 6.

## 1. Contexto

Efeito de acerto no inimigo: em vez do corte do Undertale, um **hitmarker de FPS** (o "X" de quatro traços). Mesma duração do slash do Undertale (6 frames).

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
- **Célula (um frame):** 30×30 px.
- **Grade:** 6 coluna(s) × 1 linha(s).
- **Tamanho total do PNG:** **180×30 px** (1× nativo; o jogo desenha a 2×).
- **Pivô (`origin`):** [15, 15] dentro de cada célula.
- Opcional: 2ª linha `crit` em vermelho `#FF0000`.
- **Do gerador de imagem:** peça em alta resolução com fundo `#FF00FF`; o arquivo entregue é o pequeno, já reduzido e recortado (ver "Do gerador ao arquivo final").

**Montagem do sheet:**

| Linha | Animação | Frames |
| :---: | :--- | :--- |
| 0 | `hit` | 6: surgindo do centro (1–3), cheio (4), sumindo (5–6) |

## 4. Paleta

| Uso | Hex |
| :--- | :--- |
| Hitmarker | `#FFFFFF` |
| Crítico (opcional) | `#FF0000` |

## 5. Referência atual

Nenhuma.

## 6. Pacote de envio (até 5 anexos)

Numa conversa nova, anexe **nesta ordem** e cole o prompt da seção 7:

| # | Anexo | Onde está | Para quê |
| :---: | :--- | :--- | :--- |
| md | Esta ficha | [`E05_ui_slash.md`](E05_ui_slash.md) | especificação completa |
| 1 | Template da grade/canvas | [`templates/E05_ui_slash.png`](templates/E05_ui_slash.png) | layout exato: proporção, linhas, colunas, pivô |
| 2 | Cartão de estilo | [`cards/E05_ui_slash.png`](cards/E05_ui_slash.png) | densidade de pixel (âncora), paleta, tamanho e regras numa imagem |
| 3 | Undertale — slash effect | The Spriters Resource → Undertale (só referência: salve em `assets/_ref/`, git-ignored) | duração e escala |
| 4 | Foto/ilustração real: fps hitmarker | busca de imagens: "fps hitmarker" | o X do hitmarker |

Referências são **só entrada**: material do Undertale ou de terceiros fica em `assets/_ref/` (git-ignored) e nunca é commitado nem copiado pixel a pixel.

## 7. Prompt (autocontido)

Cole inteiro. Ele repete as regras de estilo, a paleta e o formato, e explica cada anexo. Se não enviar alguma imagem, apague a linha dela em "Attached".

```text
You are drawing game art for nUndertale, a parody of the game Undertale. Rules: true pixel art in Undertale's style; every art pixel is one uniform square block; 1-pixel dark outline; flat colors with at most one shadow tone per color; no anti-aliasing, gradients, blur, painted texture or semi-transparent edges, including where the drawing meets the background. Match the pixel density of the anchor sprite on the style card: a human-sized overworld character is about 20x30 art pixels. Render the image large, but keep that number of art pixels. Use only the hex colors listed in this prompt. Adult proportions, nothing explicit, no real people, no brand logos, no text except exact words given here, no watermark or signature.

Attached: the spec file E05_ui_slash.md (full details) and these images, in order:
- image 1: layout template: copy its canvas proportions, rows, columns and cells exactly (one drawing per gray cell, white cells empty, each drawing centered in its cell); never draw its grid, gray cells, red marks or labels
- image 2: style card: match its pixel density (anchor sprite), palette, size and rules; never draw the card itself; the frog on it is only a pixel-density example, do not draw a frog or copy its features
- image 3: reference for duration and scale only
- image 4: reference for hitmarker X only
Draw a new image; never copy the reference images.

Tiny pixel art game element in the style of Undertale, crisp hard pixels, no anti-aliasing, no gradients, flat colors, high contrast, isolated on a solid flat #FF00FF magenta background, no watermark. Six-frame animation strip of a classic FPS hitmarker: four short diagonal white strokes forming an X around an empty center, growing outward from the center, full, then fading out, equal 30x30 cells in one row.

Use only these colors: #FFFFFF (Hitmarker), #FF0000 (Crítico).
Output: one image with the whole sheet laid out exactly like image 1 (6 columns x 1 rows of equal cells), on a flat, exact #FF00FF magenta background with no noise.
Avoid: blurry, anti-aliased, soft shading, smooth gradient, 3D render, realistic, photo, painterly, noisy texture, random dithering, jpeg artifacts, watermark, signature, brand logo, real person, real face, child proportions, baby face, nsfw, suggestive, cropped, cut off, inconsistent sizes between frames.
```

## 8. JSON

Salvar como `assets/ui/slash.json`. `frames` padrão 1, `fps` padrão 4, `loop` padrão `true`; animação sem loop congela no último frame; anim inexistente cai em `idle`.

```json
{
  "image": "slash.png",
  "cell": [30, 30],
  "origin": [15, 15],
  "anims": {
    "hit": { "row": 0, "frames": 6, "fps": 15, "loop": false }
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

- O centro fica vazio em todos os frames.

## 10. Checklist de aceite

- [ ] O PNG tem exatamente 180×30 px
- [ ] Sem anti-aliasing e sem cores fora da paleta da seção 4
- [ ] Fundo transparente (magenta removido)
- [ ] JSON válido ao lado do PNG, com os nomes de animação da seção 3
- [ ] Reconhecível como hitmarker de FPS.
- [ ] Registrado em `assets/PLACEHOLDERS.md` se não for a arte final
