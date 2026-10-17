# E11 · `assets/ui/hud.png` — Peças do HUD (opcional)

> Ficha autocontida: tudo o que é preciso para gerar e entregar esta imagem. Índice e regras gerais em [README.md](README.md).

| Campo | Valor |
| :--- | :--- |
| Arquivo final | `assets/ui/hud.png` |
| Prioridade | P0 — Ruins |
| Onde aparece | Batalha / diálogo. |
| O código já lê? | Não. Opcional. |

## Envio (resumo)

Conversa nova no gerador → enviar **4 anexos**: `E11_ui_hud.md` + `templates/E11_ui_hud.png` + `cards/E11_ui_hud.png` + *Undertale — battle HUD* → colar o prompt da seção 7. Detalhes de cada anexo na seção 6.

## 1. Contexto

Barra de HP (amarelo `#FFFF00` sobre vermelho `#FF0000`), overlay roxo do FUR e rótulo "FUR". **O código já desenha bem com primitivas** — só faça se quiser um visual desenhado à mão.

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
- **Tamanho:** 60×10 px (1× nativo; o jogo mostra a 120×20).
- **Sheet:** não — imagem única.
- Imagem única.
- **Do gerador de imagem:** peça em alta resolução com fundo `#FF00FF`; o arquivo entregue é o pequeno, já reduzido e recortado (ver "Do gerador ao arquivo final").

## 4. Paleta

| Uso | Hex |
| :--- | :--- |
| Branco | `#FFFFFF` |
| Preto | `#000000` |

## 5. Referência atual

Nenhuma; desenhado em código.

## 6. Pacote de envio (até 5 anexos)

Numa conversa nova, anexe **nesta ordem** e cole o prompt da seção 7:

| # | Anexo | Onde está | Para quê |
| :---: | :--- | :--- | :--- |
| md | Esta ficha | [`E11_ui_hud.md`](E11_ui_hud.md) | especificação completa |
| 1 | Template da grade/canvas | [`templates/E11_ui_hud.png`](templates/E11_ui_hud.png) | layout exato: proporção, linhas, colunas, pivô |
| 2 | Cartão de estilo | [`cards/E11_ui_hud.png`](cards/E11_ui_hud.png) | densidade de pixel (âncora), paleta, tamanho e regras numa imagem |
| 3 | Undertale — battle HUD | The Spriters Resource → Undertale (só referência: salve em `assets/_ref/`, git-ignored) | tamanho da barra de HP |

Referências são **só entrada**: material do Undertale ou de terceiros fica em `assets/_ref/` (git-ignored) e nunca é commitado nem copiado pixel a pixel.

## 7. Prompt (autocontido)

Cole inteiro. Ele repete as regras de estilo, a paleta e o formato, e explica cada anexo. Se não enviar alguma imagem, apague a linha dela em "Attached".

```text
You are drawing game art for nUndertale, a parody of the game Undertale. Rules: true pixel art in Undertale's style; every art pixel is one uniform square block; 1-pixel dark outline; flat colors with at most one shadow tone per color; no anti-aliasing, gradients, blur, painted texture or semi-transparent edges, including where the drawing meets the background. Match the pixel density of the anchor sprite on the style card: a human-sized overworld character is about 20x30 art pixels. Render the image large, but keep that number of art pixels. Use only the hex colors listed in this prompt. Adult proportions, nothing explicit, no real people, no brand logos, no text except exact words given here, no watermark or signature.

Attached: the spec file E11_ui_hud.md (full details) and these images, in order:
- image 1: canvas template: copy only its proportions and framing; never draw its gray boxes, guide lines or labels
- image 2: style card: match its pixel density (anchor sprite), palette, size and rules; never draw the card itself; the frog on it is only a pixel-density example, do not draw a frog or copy its features
- image 3: reference for HP bar size only
Draw a new image; never copy the reference images.

Tiny pixel art game element in the style of Undertale, crisp hard pixels, no anti-aliasing, no gradients, flat colors, high contrast, isolated on a solid flat #FF00FF magenta background, no watermark. Pixel art HUD pieces on black: a short horizontal HP bar segment in yellow #FFFF00, the same segment in red #FF0000, a purple overlay segment, and the small label "FUR" in chunky white pixel letters.

Use only these colors: #FFFFFF (Branco), #000000 (Preto).
Output: one image with aspect ratio 60:10, on a flat, exact #FF00FF magenta background with no noise.
Avoid: blurry, anti-aliased, soft shading, smooth gradient, 3D render, realistic, photo, painterly, noisy texture, random dithering, jpeg artifacts, watermark, signature, brand logo, real person, real face, child proportions, baby face, nsfw, suggestive, cropped, cut off, inconsistent sizes between frames.
```

## 8. Do gerador ao arquivo final

1. Numa conversa nova do gerador, envie o pacote da seção 6 (o `.md` + até 4 imagens) e cole o prompt da seção 7.
2. Se o gerador errar a grade, gere **uma pose/frame por vez** (comece pelo parado de frente), aprove, e use o frame aprovado como referência para os outros; monte o sheet à mão. Alternativa: geradores nativos de pixel art (PixelLab, Retro Diffusion para Aseprite) já entregam no tamanho real.
3. No Aseprite / LibreSprite / Pixelorama: reduza para o tamanho nativo com **nearest neighbor** (ou redesenhe por cima), quantize para a paleta da seção 4, limpe o contorno para 1 px.
4. Monte na grade da seção 3, alinhe o pivô com uma guia, troque o magenta por transparência.
5. Exporte no formato da seção 3, escreva o JSON (seção "JSON", quando houver) e registre o arquivo em `assets/PLACEHOLDERS.md` (fonte/gerador, autor, licença).

## 9. Checklist de aceite

- [ ] O arquivo tem exatamente 60×10 px
- [ ] Sem anti-aliasing e sem cores fora da paleta da seção 4
- [ ] Fundo transparente (magenta removido)
- [ ] Encaixa nas posições atuais do código.
- [ ] Registrado em `assets/PLACEHOLDERS.md` se não for a arte final
