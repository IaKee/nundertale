# C2 · `assets/portraits/floofy_*.png` — Retratos de diálogo — Floofy

> Ficha autocontida: tudo o que é preciso para gerar e entregar esta imagem. Índice e regras gerais em [README.md](README.md).

| Campo | Valor |
| :--- | :--- |
| Arquivo final | `assets/portraits/floofy_*.png` |
| Prioridade | P0 — Ruins |
| Onde aparece | Caixas de diálogo em que o personagem fala. |
| O código já lê? | Não (sem loader de retratos ainda). Entrega em arquivos soltos. |

## Envio (resumo)

Conversa nova no gerador → enviar **5 anexos**: `C02_portraits_floofy.md` + `templates/C02_portraits_floofy.png` + `cards/C02_portraits_floofy.png` + `assets/sprites/battle/floofy.png` + *Undertale — Flowey dialogue face sprites* → colar o prompt da seção 7. Detalhes de cada anexo na seção 6.

## 1. Contexto

Floofy (ver A5/B3): flor com cabeça de fursuit de cabritinho e zíper no meio da cara. Fofo e falso; depois sem limites. Se as pétalas forem amarelas no B3, use o mesmo aqui.

## 2. Regras que valem para esta imagem

- **Escala:** arte em **1× nativo** (tela cheia = 320×240). O jogo desenha a **2×** com filtro *point* num canvas virtual de 640×480. Todos os tamanhos desta ficha são 1×.
- **Pixel:** sem anti-aliasing, sem gradiente suave, sem blur, sem sub-pixel.
- **Sprite sheet:** grade fixa, células do mesmo tamanho, sem espaçamento nem margem. **Uma linha por animação**, frames da esquerda para a direita **começando na coluna 0**. O loader não sabe começar em outra coluna, então **estados/variantes diferentes vão em linhas diferentes**. Célula vazia = transparente.
- **Pivô (`origin`):** o mesmo ponto (pé / base central) em todas as células.
- **Conteúdo:** nada explícito; proporções **adultas**; nenhum rosto real; nenhuma marca real (nada de logo Doritos / Mountain Dew; use "CITRUS BLAST", chips sem marca); todo texto dentro da arte em **inglês**.
- **Undertale:** pode servir de referência de tamanho/estilo, mas nada copiado vai para o repositório.
- Batalha é **só branco `#FFFFFF` e preto `#000000`**, como a Toriel e o Flowey do Undertale: traço branco; materiais claros (pelo, rosto, cabeça de fantasia) preenchidos de branco, materiais escuros (roupa) de preto.
- **Contorno branco fechado em volta da silhueta inteira**, também em volta da roupa preta, do cabelo, das orelhas e do rabo: nenhum preto do personagem encosta no fundo (guia de estilo §3.1). A keyline preta de 2 px por fora é aplicada depois por `tools/art_prompts/battle_outline.py`.
- Fora da silhueta = **transparente**. O fundo das batalhas comuns tem uma grade verde que não pode aparecer através do personagem, por isso o interior precisa ser todo opaco (branco ou preto).
- Nada de cinza: "meio-tom" se faz com pixels brancos espaçados.
- Cor só em momentos especiais, e só onde esta ficha disser.

## 3. Formato de output esperado

- **Tipo:** PNG RGBA 32-bit, fundo transparente.
- **Tamanho de cada arquivo:** 50×50 px (1× nativo; o jogo mostra a 100×100).
- **Arquivos a entregar:**

| Arquivo | Expressão |
| :--- | :--- |
| `assets/portraits/floofy_cute.png` | olhos grandes e brilhantes de mascote, sem boca |
| `assets/portraits/floofy_smug.png` | olhos semicerrados + um sorriso torto de 1 linha: a única boca do Floofy, só porque o deboche não lê sem ela |
| `assets/portraits/floofy_sinister.png` | zíper aberto: só escuridão + dois olhos brancos |

- **Um PNG por expressão**, 50×50 cada, nomes exatamente como na tabela.
- Gere todas as expressões **numa imagem só** (mesma seed) para consistência e depois recorte.
- Enquadramento: cabeça e ombros, cabeça ocupando ~70% da altura, **mesma posição** em todos.
- Preto opaco dentro da silhueta, transparente fora (o retrato fica dentro da caixa preta de diálogo).
- **Do gerador de imagem:** peça em alta resolução com fundo `#FF00FF`; o arquivo entregue é o pequeno, já reduzido e recortado (ver "Do gerador ao arquivo final").

## 4. Paleta

| Uso | Hex |
| :--- | :--- |
| Traço | `#FFFFFF` |
| Preenchimento interno (opaco) | `#000000` |

## 5. Referência atual

Nenhuma. Referência de estilo: os retratos da Toriel no Undertale (traço branco).

## 6. Pacote de envio (até 5 anexos)

Numa conversa nova, anexe **nesta ordem** e cole o prompt da seção 7:

| # | Anexo | Onde está | Para quê |
| :---: | :--- | :--- | :--- |
| md | Esta ficha | [`C02_portraits_floofy.md`](C02_portraits_floofy.md) | especificação completa |
| 1 | Template da grade/canvas | [`templates/C02_portraits_floofy.png`](templates/C02_portraits_floofy.png) | layout exato: proporção, linhas, colunas, pivô |
| 2 | Cartão de estilo | [`cards/C02_portraits_floofy.png`](cards/C02_portraits_floofy.png) | densidade de pixel (âncora), paleta, tamanho e regras numa imagem |
| 3 | Saída aprovada da ficha B3 | `assets/sprites/battle/floofy.png` (depois de aprovada) | mesmo personagem / mesmo design |
| 4 | Undertale — Flowey dialogue face sprites | The Spriters Resource → Undertale (só referência: salve em `assets/_ref/`, git-ignored) | enquadramento e expressões |

Referências são **só entrada**: material do Undertale ou de terceiros fica em `assets/_ref/` (git-ignored) e nunca é commitado nem copiado pixel a pixel.

## 7. Prompt (autocontido)

Cole inteiro. Ele repete as regras de estilo, a paleta e o formato, e explica cada anexo. Se não enviar alguma imagem, apague a linha dela em "Attached".

```text
You are drawing game art for nUndertale, a parody of the game Undertale. Rules: true pixel art in Undertale's style; every art pixel is one uniform square block; 1-pixel dark outline; flat colors with at most one shadow tone per color; no anti-aliasing, gradients, blur, painted texture or semi-transparent edges, including where the drawing meets the background. Match the pixel density of the anchor sprite on the style card: a human-sized overworld character is about 20x30 art pixels. Render the image large, but keep that number of art pixels. Use only the hex colors listed in this prompt. Adult proportions, nothing explicit, no real people, no brand logos, no text except exact words given here, no watermark or signature.

Attached: the spec file C02_portraits_floofy.md (full details) and these images, in order:
- image 1: canvas template: copy only its proportions and framing; never draw its gray boxes, guide lines or labels
- image 2: style card: match its pixel density (anchor sprite), palette, size and rules; never draw the card itself; the frog on it is only a pixel-density example, do not draw a frog or copy its features
- image 3: the approved design of the same character: keep its design exactly
- image 4: reference for framing and expressions only
Draw a new image; never copy the reference images.

Undertale battle sprite, monochrome white and black pixel art (only #FFFFFF and #000000): white 1-pixel line art, light materials such as fur, faces and costume heads filled solid white, dark materials such as clothes filled solid black, a continuous unbroken white 1-pixel outline around the ENTIRE outer edge of the silhouette (also around black clothes, hair, ears and tail, so no black part of the figure ever touches the background), a few long continuous white lines for folds and seams instead of scattered dots, the whole silhouette fully opaque, bold readable silhouette, hand-drawn pixel art, no anti-aliasing, no gray, no gradients, front-facing, centered, isolated on a solid flat #FF00FF magenta background, no watermark. Dialogue portrait sheet, 3 portraits in one row, identical framing (50x50 pixels each): a flower whose face is a baby-goat mascot costume head with plastic eyes and a silver zipper running down the middle of the face from the top of the head to the chin, with the pull tab at the chin; the costume has no mouth. Expressions: cute (big shiny innocent eyes, no mouth), smug (half-lidded eyes and one thin crooked grin line, the only mouth this character ever shows), sinister with the zipper pulled open showing only darkness and two white dot eyes. No text.

Use only these colors: #FFFFFF (Traço), #000000 (Preenchimento interno).
Output: one image with 3 portraits side by side, framed like image 1, on a flat, exact #FF00FF magenta background with no noise.
Avoid: blurry, anti-aliased, soft shading, smooth gradient, 3D render, realistic, photo, painterly, noisy texture, random dithering, jpeg artifacts, watermark, signature, brand logo, real person, real face, child proportions, baby face, nsfw, suggestive, cropped, cut off, inconsistent sizes between frames.
```

## 8. Do gerador ao arquivo final

1. Numa conversa nova do gerador, envie o pacote da seção 6 (o `.md` + até 4 imagens) e cole o prompt da seção 7.
2. Se o gerador errar a grade, gere **uma pose/frame por vez** (comece pelo parado de frente), aprove, e use o frame aprovado como referência para os outros; monte o sheet à mão. Alternativa: geradores nativos de pixel art (PixelLab, Retro Diffusion para Aseprite) já entregam no tamanho real.
3. No Aseprite / LibreSprite / Pixelorama: reduza para o tamanho nativo com **nearest neighbor** (ou redesenhe por cima), quantize para a paleta da seção 4, limpe o contorno para 1 px.
4. Monte na grade da seção 3, alinhe o pivô com uma guia, troque o magenta por transparência.
5. Exporte no formato da seção 3, escreva o JSON (seção "JSON", quando houver) e registre o arquivo em `assets/PLACEHOLDERS.md` (fonte/gerador, autor, licença).

**Específico desta imagem:**

- Alinhe olhos e queixo na mesma altura em todos os arquivos (troca de expressão não pode "pular").

## 9. Checklist de aceite

- [ ] Cada arquivo tem exatamente 50×50 px
- [ ] Sem anti-aliasing e sem cores fora da paleta da seção 4
- [ ] Fundo transparente (magenta removido)
- [ ] Expressões distinguíveis em 50×50.
- [ ] Mesmo personagem em todos.
- [ ] Registrado em `assets/PLACEHOLDERS.md` se não for a arte final
