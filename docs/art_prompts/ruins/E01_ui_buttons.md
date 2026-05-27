# E1 · `assets/ui/buttons.png` — Botões de batalha FIGHT / ACT / ITEM / MERCY

> Ficha autocontida: tudo o que é preciso para gerar e entregar esta imagem. Índice e regras gerais em [README.md](README.md).

| Campo | Valor |
| :--- | :--- |
| Arquivo final | `assets/ui/buttons.png` |
| JSON | `assets/ui/buttons.json` |
| Prioridade | P0 — Ruins · **núcleo da vertical slice** |
| Onde aparece | Toda batalha. |
| O código já lê? | Não (hoje: retângulo + texto em `BattleSystem::drawButtons`). |

## Envio (resumo)

Conversa nova no gerador → enviar **5 anexos**: `E01_ui_buttons.md` + `templates/E01_ui_buttons.png` + `cards/E01_ui_buttons.png` + *Undertale — battle buttons (FIGHT/ACT/ITEM/MERCY)* + *Foto/ilustração real: fps crosshair speech bubble chip bag white flag pixel icons* → colar o prompt da seção 7. Detalhes de cada anexo na seção 6.

## 1. Contexto

Os quatro botões do menu de batalha (o código desenha cada um em 110×42 = 2× de 55×21). Normal = laranja `#FF7F27`, selecionado = amarelo `#FFFF00`, MERCY travado = cinza `#808080` (cores de `include/core/Constants.hpp`). No botão selecionado o código desenha a **alma vermelha no lugar do ícone** (BUI-006, igual ao Undertale).

Ícones paródia: FIGHT = mira de FPS, ACT = balão de fala, ITEM = saquinho de salgadinho, MERCY = bandeira branca com uma patinha. Letras grossas "feitas à mão".

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
- **Célula (um frame):** 55×21 px.
- **Grade:** 1 coluna(s) × 9 linha(s).
- **Tamanho total do PNG:** **55×189 px** (1× nativo; o jogo desenha a 2×).
- **Pivô (`origin`):** [0, 0] dentro de cada célula.
- Um botão por linha (o loader seleciona pela linha).
- Borda de 1 px, ícone em x 2–15, texto à direita, interior preto opaco.
- Nas linhas **selected** deixe a área do ícone **vazia (preta)** — a alma é desenhada ali pelo código.
- **Do gerador de imagem:** peça em alta resolução com fundo `#FF00FF`; o arquivo entregue é o pequeno, já reduzido e recortado (ver "Do gerador ao arquivo final").

**Montagem do sheet:**

| Linha | Animação | Conteúdo |
| :---: | :--- | :--- |
| 0 | `fight` | FIGHT laranja com ícone |
| 1 | `fight_selected` | FIGHT amarelo, sem ícone |
| 2 | `act` | ACT laranja |
| 3 | `act_selected` | ACT amarelo, sem ícone |
| 4 | `item` | ITEM laranja |
| 5 | `item_selected` | ITEM amarelo, sem ícone |
| 6 | `mercy` | MERCY laranja |
| 7 | `mercy_selected` | MERCY amarelo, sem ícone |
| 8 | `mercy_locked` | MERCY cinza com ícone (P1) |

## 4. Paleta

| Uso | Hex |
| :--- | :--- |
| Normal | `#FF7F27` |
| Selecionado | `#FFFF00` |
| Travado | `#808080` |
| Interior | `#000000` |

## 5. Referência atual

Nenhuma imagem; o desenho atual é feito em código.

## 6. Pacote de envio (até 5 anexos)

Numa conversa nova, anexe **nesta ordem** e cole o prompt da seção 7:

| # | Anexo | Onde está | Para quê |
| :---: | :--- | :--- | :--- |
| md | Esta ficha | [`E01_ui_buttons.md`](E01_ui_buttons.md) | especificação completa |
| 1 | Template da grade/canvas | [`templates/E01_ui_buttons.png`](templates/E01_ui_buttons.png) | layout exato: proporção, linhas, colunas, pivô |
| 2 | Cartão de estilo | [`cards/E01_ui_buttons.png`](cards/E01_ui_buttons.png) | densidade de pixel (âncora), paleta, tamanho e regras numa imagem |
| 3 | Undertale — battle buttons (FIGHT/ACT/ITEM/MERCY) | The Spriters Resource → Undertale (só referência: salve em `assets/_ref/`, git-ignored) | proporção, borda e posição do ícone |
| 4 | Foto/ilustração real: fps crosshair speech bubble chip bag white flag pixel icons | busca de imagens: "fps crosshair speech bubble chip bag white flag pixel icons" | ícones dos botões |

Referências são **só entrada**: material do Undertale ou de terceiros fica em `assets/_ref/` (git-ignored) e nunca é commitado nem copiado pixel a pixel.

## 7. Prompt (autocontido)

Cole inteiro. Ele repete as regras de estilo, a paleta e o formato, e explica cada anexo. Se não enviar alguma imagem, apague a linha dela em "Attached".

```text
You are drawing game art for nUndertale, a parody of the game Undertale. Rules: true pixel art in Undertale's style; every art pixel is one uniform square block; 1-pixel dark outline; flat colors with at most one shadow tone per color; no anti-aliasing, gradients, blur, painted texture or semi-transparent edges, including where the drawing meets the background. Match the pixel density of the anchor sprite on the style card: a human-sized overworld character is about 20x30 art pixels. Render the image large, but keep that number of art pixels. Use only the hex colors listed in this prompt. Adult proportions, nothing explicit, no real people, no brand logos, no text except exact words given here, no watermark or signature.

Attached: the spec file E01_ui_buttons.md (full details) and these images, in order:
- image 1: layout template: copy its canvas proportions, rows, columns and cells exactly (one drawing per gray cell, white cells empty, each drawing centered in its cell); never draw its grid, gray cells, red marks or labels
- image 2: style card: match its pixel density (anchor sprite), palette, size and rules; never draw the card itself; the frog on it is only a pixel-density example, do not draw a frog or copy its features
- image 3: reference for proportions, border and icon position only
- image 4: reference for button icons only
Draw a new image; never copy the reference images.

Tiny pixel art game element in the style of Undertale, crisp hard pixels, no anti-aliasing, no gradients, flat colors, high contrast, isolated on a solid flat #FF00FF magenta background, no watermark. Undertale-style battle menu buttons, each button 55x21 pixels: a 1-pixel rectangular border, a small icon on the left and chunky hand-drawn bold capital letters on the right, black fill. Nine buttons stacked vertically in equal cells: "FIGHT" orange #FF7F27 with an FPS crosshair icon; "FIGHT" yellow #FFFF00 with the icon area empty; "ACT" orange with a speech-bubble icon; "ACT" yellow with the icon area empty; "ITEM" orange with a small chip-bag icon; "ITEM" yellow with the icon area empty; "MERCY" orange with a white flag icon that has a paw print; "MERCY" yellow with the icon area empty; "MERCY" gray #808080 with the flag icon.

Use only these colors: #FF7F27 (Normal), #FFFF00 (Selecionado), #808080 (Travado), #000000 (Interior).
Output: one image with the whole sheet laid out exactly like image 1 (1 columns x 9 rows of equal cells), on a flat, exact #FF00FF magenta background with no noise.
Avoid: blurry, anti-aliased, soft shading, smooth gradient, 3D render, realistic, photo, painterly, noisy texture, random dithering, jpeg artifacts, watermark, signature, brand logo, real person, real face, child proportions, baby face, nsfw, suggestive, cropped, cut off, inconsistent sizes between frames.
```

## 8. JSON

Salvar como `assets/ui/buttons.json`. `frames` padrão 1, `fps` padrão 4, `loop` padrão `true`; animação sem loop congela no último frame; anim inexistente cai em `idle`.

```json
{
  "image": "buttons.png",
  "cell": [55, 21],
  "origin": [0, 0],
  "anims": {
    "fight": { "row": 0, "frames": 1 },
    "fight_selected": { "row": 1, "frames": 1 },
    "act": { "row": 2, "frames": 1 },
    "act_selected": { "row": 3, "frames": 1 },
    "item": { "row": 4, "frames": 1 },
    "item_selected": { "row": 5, "frames": 1 },
    "mercy": { "row": 6, "frames": 1 },
    "mercy_selected": { "row": 7, "frames": 1 },
    "mercy_locked": { "row": 8, "frames": 1 }
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

- A IA vai errar as letras: use a fonte do jogo (`assets/fonts/8bitoperator.ttf`) ou desenhe à mão com 7 px de altura.
- Ícones com no máximo 12×12.

## 10. Checklist de aceite

- [ ] O PNG tem exatamente 55×189 px
- [ ] Sem anti-aliasing e sem cores fora da paleta da seção 4
- [ ] Fundo transparente (magenta removido)
- [ ] JSON válido ao lado do PNG, com os nomes de animação da seção 3
- [ ] Texto exatamente FIGHT / ACT / ITEM / MERCY.
- [ ] Área do ícone vazia nas linhas *selected*.
- [ ] Registrado em `assets/PLACEHOLDERS.md` se não for a arte final
