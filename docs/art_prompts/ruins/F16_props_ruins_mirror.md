# F16 · `assets/props/ruins_mirror.png` — Espelho do corredor

> Ficha autocontida: tudo o que é preciso para gerar e entregar esta imagem. Índice e regras gerais em [README.md](README.md).

| Campo | Valor |
| :--- | :--- |
| Arquivo final | `assets/props/ruins_mirror.png` |
| JSON | `assets/props/ruins_mirror.json` |
| Prioridade | P0 — Ruins |
| Onde aparece | R11 — corredor. |
| O código já lê? | Não. Depende do loader de mapas/objetos (OVW-003). |

## Envio (resumo)

Conversa nova no gerador → enviar **4 anexos**: `F16_props_ruins_mirror.md` + `templates/F16_props_ruins_mirror.png` + `cards/F16_props_ruins_mirror.png` + *Undertale — Toriel's hallway mirror* → colar o prompt da seção 7. Detalhes de cada anexo na seção 6.

## 1. Contexto

Espelho alto com moldura roxa ornamentada. Examinar: `* It's you!` → com FUR ≥ 20: `* It's... still you. With ears.` O reflexo do Pepe é desenhado pela cena, não no prop.

**Conceito da área:** convenção furry de 2014 abandonada dentro das ruínas roxas do Undertale.

## 2. Regras que valem para esta imagem

- **Escala:** arte em **1× nativo** (tela cheia = 320×240). O jogo desenha a **2×** com filtro *point* num canvas virtual de 640×480. Todos os tamanhos desta ficha são 1×.
- **Pixel:** sem anti-aliasing, sem gradiente suave, sem blur, sem sub-pixel.
- **Sprite sheet:** grade fixa, células do mesmo tamanho, sem espaçamento nem margem. **Uma linha por animação**, frames da esquerda para a direita **começando na coluna 0**. O loader não sabe começar em outra coluna, então **estados/variantes diferentes vão em linhas diferentes**. Célula vazia = transparente.
- **Pivô (`origin`):** o mesmo ponto (pé / base central) em todas as células.
- **Conteúdo:** nada explícito; proporções **adultas**; nenhum rosto real; nenhuma marca real (nada de logo Doritos / Mountain Dew; use "CITRUS BLAST", chips sem marca); todo texto dentro da arte em **inglês**.
- **Undertale:** pode servir de referência de tamanho/estilo, mas nada copiado vai para o repositório.
- Mesma vista 3/4 e mesma paleta do tileset das Ruins.
- Texto em placas/banners: gere **sem texto** e escreva à mão depois em fonte pixel de 5 px (a IA não escreve legível nesse tamanho). Texto sempre em inglês, exatamente como indicado.

## 3. Formato de output esperado

- **Tipo:** PNG RGBA 32-bit, fundo transparente.
- **Célula (um frame):** 20×40 px.
- **Grade:** 1 coluna(s) × 2 linha(s).
- **Tamanho total do PNG:** **20×80 px** (1× nativo; o jogo desenha a 2×).
- **Pivô (`origin`):** [10, 39] dentro de cada célula.
- Estados/variantes em linhas separadas; frames de animação na mesma linha.
- **Do gerador de imagem:** peça em alta resolução com fundo `#FF00FF`; o arquivo entregue é o pequeno, já reduzido e recortado (ver "Do gerador ao arquivo final").

**Montagem do sheet:**

| Linha | Animação | Estado |
| :---: | :--- | :--- |
| 0 | `normal` | vidro frio |
| 1 | `fur` | brilho rosado `#FFB3E6` |

## 4. Paleta

| Uso | Hex |
| :--- | :--- |
| Base escura | `#3B1F5C` |
| Base média | `#5E3A87` |
| Acento neon | `#7CFF3A` |
| Realce | `#F4E3FF` |
| Banners | `#C9A0DC` |
| Cortina 18+ | `#111111` |
| Tela CRT | `#1E3FBF` |
| Roxos intermediários (opcionais) | `#4A2A70` / `#7A55A8` / `#9C7CC4` |

## 5. Referência atual

Nenhuma.

## 6. Pacote de envio (até 5 anexos)

Numa conversa nova, anexe **nesta ordem** e cole o prompt da seção 7:

| # | Anexo | Onde está | Para quê |
| :---: | :--- | :--- | :--- |
| md | Esta ficha | [`F16_props_ruins_mirror.md`](F16_props_ruins_mirror.md) | especificação completa |
| 1 | Template da grade/canvas | [`templates/F16_props_ruins_mirror.png`](templates/F16_props_ruins_mirror.png) | layout exato: proporção, linhas, colunas, pivô |
| 2 | Cartão de estilo | [`cards/F16_props_ruins_mirror.png`](cards/F16_props_ruins_mirror.png) | densidade de pixel (âncora), paleta, tamanho e regras numa imagem |
| 3 | Undertale — Toriel's hallway mirror | The Spriters Resource → Undertale (só referência: salve em `assets/_ref/`, git-ignored) | escala e moldura |

Referências são **só entrada**: material do Undertale ou de terceiros fica em `assets/_ref/` (git-ignored) e nunca é commitado nem copiado pixel a pixel.

## 7. Prompt (autocontido)

Cole inteiro. Ele repete as regras de estilo, a paleta e o formato, e explica cada anexo. Se não enviar alguma imagem, apague a linha dela em "Attached".

```text
You are drawing game art for nUndertale, a parody of the game Undertale. Rules: true pixel art in Undertale's style; every art pixel is one uniform square block; 1-pixel dark outline; flat colors with at most one shadow tone per color; no anti-aliasing, gradients, blur, painted texture or semi-transparent edges, including where the drawing meets the background. Match the pixel density of the anchor sprite on the style card: a human-sized overworld character is about 20x30 art pixels. Render the image large, but keep that number of art pixels. Use only the hex colors listed in this prompt. Adult proportions, nothing explicit, no real people, no brand logos, no text except exact words given here, no watermark or signature.

Attached: the spec file F16_props_ruins_mirror.md (full details) and these images, in order:
- image 1: layout template: copy its canvas proportions, rows, columns and cells exactly (one drawing per gray cell, white cells empty, feet or base on the red line); never draw its grid, gray cells, red marks or labels
- image 2: style card: match its pixel density (anchor sprite), palette, size and rules; never draw the card itself; the frog on it is only a pixel-density example, do not draw a frog or copy its features
- image 3: reference for scale and frame only
Draw a new image; never copy the reference images.

Single pixel art prop for a top-down RPG in the style of Undertale's Ruins, 3/4 top-down view, crisp 1-pixel dark outline, flat cel shading, no anti-aliasing, no gradients, no blur, limited palette (#3B1F5C, #5E3A87, #7CFF3A, #F4E3FF, #C9A0DC plus at most three in-between purples), isolated on a solid flat #FF00FF magenta background, no text, no watermark. Tall standing hallway mirror with an ornate purple frame, two states stacked vertically in equal 20x40 cells: normal cool reflective glass; glass with a soft pink glow #FFB3E6.

Use only these colors: #3B1F5C (Base escura), #5E3A87 (Base média), #7CFF3A (Acento neon), #F4E3FF (Realce), #C9A0DC (Banners), #111111 (Cortina 18+), #1E3FBF (Tela CRT), #4A2A70 (Roxos intermediários), #7A55A8 (Roxos intermediários), #9C7CC4 (Roxos intermediários).
Output: one image with the whole sheet laid out exactly like image 1 (1 columns x 2 rows of equal cells), on a flat, exact #FF00FF magenta background with no noise.
Avoid: blurry, anti-aliased, soft shading, smooth gradient, 3D render, realistic, photo, painterly, noisy texture, random dithering, jpeg artifacts, watermark, signature, brand logo, real person, real face, child proportions, baby face, nsfw, suggestive, cropped, cut off, inconsistent sizes between frames.
```

## 8. JSON

Salvar como `assets/props/ruins_mirror.json`. `frames` padrão 1, `fps` padrão 4, `loop` padrão `true`; animação sem loop congela no último frame; anim inexistente cai em `idle`.

```json
{
  "image": "ruins_mirror.png",
  "cell": [20, 40],
  "origin": [10, 39],
  "anims": {
    "normal": { "row": 0, "frames": 1 },
    "fur": { "row": 1, "frames": 1 }
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

- Mesma luz (de cima/esquerda) e contorno do tileset F1.

## 10. Checklist de aceite

- [ ] O PNG tem exatamente 20×80 px
- [ ] Sem anti-aliasing e sem cores fora da paleta da seção 4
- [ ] Fundo transparente (magenta removido)
- [ ] JSON válido ao lado do PNG, com os nomes de animação da seção 3
- [ ] Combina com o tileset F1 lado a lado.
- [ ] Registrado em `assets/PLACEHOLDERS.md` se não for a arte final
