# D12 · `assets/bullets/grass_blade.png` — Projétil `grass_blade` — Grasstoid — padrão `grass_blades`

> Ficha autocontida: tudo o que é preciso para gerar e entregar esta imagem. Índice e regras gerais em [README.md](README.md).

| Campo | Valor |
| :--- | :--- |
| Arquivo final | `assets/bullets/grass_blade.png` |
| JSON | `assets/bullets/grass_blade.json` |
| Prioridade | P0 — Ruins |
| Onde aparece | Grasstoid — padrão `grass_blades` |
| O código já lê? | Não (hoje os projéteis são primitivas). JSON abaixo é a sugestão para quando o loader de bullets existir. |

## Envio (resumo)

Conversa nova no gerador → enviar **5 anexos**: `D12_bullet_grass_blade.md` + `templates/D12_bullet_grass_blade.png` + `cards/D12_bullet_grass_blade.png` + *Undertale — Vegetoid carrot bullets (green heal)* + *Kenney 1-Bit Pack* → colar o prompt da seção 7. Detalhes de cada anexo na seção 6.

## 1. Contexto

Lâminas de grama. As **verdes curam** (convenção do Undertale) — por isso há duas cores.

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
- **Célula (um frame):** 8×16 px.
- **Grade:** 2 coluna(s) × 2 linha(s).
- **Tamanho total do PNG:** **16×32 px** (1× nativo; o jogo desenha a 2×).
- **Pivô (`origin`):** [4, 8] dentro de cada célula.
- Pivô no centro (projéteis giram/movem pelo centro).
- Deixe ≥1 px transparente em volta do desenho.
- Cores em linhas separadas.
- **Do gerador de imagem:** peça em alta resolução com fundo `#FF00FF`; o arquivo entregue é o pequeno, já reduzido e recortado (ver "Do gerador ao arquivo final").

**Montagem do sheet:**

| Linha | Animação | Frames |
| :---: | :--- | :--- |
| 0 | `white` | 2: balanço |
| 1 | `green` | 2: balanço (verde, cura) |

## 4. Paleta

| Uso | Hex |
| :--- | :--- |
| Lâmina que fere | `#FFFFFF` |
| Lâmina que cura | `#00FF00` |

## 5. Referência atual

Nenhuma (primitivas no código).

## 6. Pacote de envio (até 5 anexos)

Numa conversa nova, anexe **nesta ordem** e cole o prompt da seção 7:

| # | Anexo | Onde está | Para quê |
| :---: | :--- | :--- | :--- |
| md | Esta ficha | [`D12_bullet_grass_blade.md`](D12_bullet_grass_blade.md) | especificação completa |
| 1 | Template da grade/canvas | [`templates/D12_bullet_grass_blade.png`](templates/D12_bullet_grass_blade.png) | layout exato: proporção, linhas, colunas, pivô |
| 2 | Cartão de estilo | [`cards/D12_bullet_grass_blade.png`](cards/D12_bullet_grass_blade.png) | densidade de pixel (âncora), paleta, tamanho e regras numa imagem |
| 3 | Undertale — Vegetoid carrot bullets (green heal) | The Spriters Resource → Undertale (só referência: salve em `assets/_ref/`, git-ignored) | convenção de verde = cura |
| 4 | Kenney 1-Bit Pack | kenney.nl (CC0) | formas 1-bit legíveis em tamanho mínimo |

Referências são **só entrada**: material do Undertale ou de terceiros fica em `assets/_ref/` (git-ignored) e nunca é commitado nem copiado pixel a pixel.

## 7. Prompt (autocontido)

Cole inteiro. Ele repete as regras de estilo, a paleta e o formato, e explica cada anexo. Se não enviar alguma imagem, apague a linha dela em "Attached".

```text
You are drawing game art for nUndertale, a parody of the game Undertale. Rules: true pixel art in Undertale's style; every art pixel is one uniform square block; 1-pixel dark outline; flat colors with at most one shadow tone per color; no anti-aliasing, gradients, blur, painted texture or semi-transparent edges, including where the drawing meets the background. Match the pixel density of the anchor sprite on the style card: a human-sized overworld character is about 20x30 art pixels. Render the image large, but keep that number of art pixels. Use only the hex colors listed in this prompt. Adult proportions, nothing explicit, no real people, no brand logos, no text except exact words given here, no watermark or signature.

Attached: the spec file D12_bullet_grass_blade.md (full details) and these images, in order:
- image 1: layout template: copy its canvas proportions, rows, columns and cells exactly (one drawing per gray cell, white cells empty, each drawing centered in its cell); never draw its grid, gray cells, red marks or labels
- image 2: style card: match its pixel density (anchor sprite), palette, size and rules; never draw the card itself; the frog on it is only a pixel-density example, do not draw a frog or copy its features
- image 3: reference for green-heals convention only
- image 4: reference for readable 1-bit shapes at tiny size
Draw a new image; never copy the reference images.

Tiny pixel art game element in the style of Undertale, crisp hard pixels, no anti-aliasing, no gradients, flat colors, high contrast, isolated on a solid flat #FF00FF magenta background, no watermark. Thin pointed grass-blade bullet, two sway frames per row, equal 8x16 cells: top row pure white, bottom row bright green #00FF00.

Use only these colors: #FFFFFF (Lâmina que fere), #00FF00 (Lâmina que cura).
Output: one image with the whole sheet laid out exactly like image 1 (2 columns x 2 rows of equal cells), on a flat, exact #FF00FF magenta background with no noise.
Avoid: blurry, anti-aliased, soft shading, smooth gradient, 3D render, realistic, photo, painterly, noisy texture, random dithering, jpeg artifacts, watermark, signature, brand logo, real person, real face, child proportions, baby face, nsfw, suggestive, cropped, cut off, inconsistent sizes between frames.
```

## 8. JSON

Salvar como `assets/bullets/grass_blade.json`. `frames` padrão 1, `fps` padrão 4, `loop` padrão `true`; animação sem loop congela no último frame; anim inexistente cai em `idle`.

```json
{
  "image": "grass_blade.png",
  "cell": [8, 16],
  "origin": [4, 8],
  "anims": {
    "white": { "row": 0, "frames": 2, "fps": 4 },
    "green": { "row": 1, "frames": 2, "fps": 4 }
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

- Núcleo sólido e centralizado; nada de pontas finas de 1 px que pareçam acertar sem tocar.

## 10. Checklist de aceite

- [ ] O PNG tem exatamente 16×32 px
- [ ] Sem anti-aliasing e sem cores fora da paleta da seção 4
- [ ] Fundo transparente (magenta removido)
- [ ] JSON válido ao lado do PNG, com os nomes de animação da seção 3
- [ ] Legível em movimento a 2×.
- [ ] Silhueta clara sobre fundo preto.
- [ ] Registrado em `assets/PLACEHOLDERS.md` se não for a arte final
