# C1 · `assets/portraits/furiell_*.png` — Retratos de diálogo — Furiell

> Ficha autocontida: tudo o que é preciso para gerar e entregar esta imagem. Índice e regras gerais em [README.md](README.md).

| Campo | Valor |
| :--- | :--- |
| Arquivo final | `assets/portraits/furiell_*.png` |
| Prioridade | P0 — Ruins |
| Onde aparece | Caixas de diálogo em que o personagem fala. |
| O código já lê? | Não (sem loader de retratos ainda). Entrega em arquivos soltos. |

## Envio (resumo)

Conversa nova no gerador → enviar **5 anexos**: `C01_portraits_furiell.md` + `templates/C01_portraits_furiell.png` + `cards/C01_portraits_furiell.png` + `assets/sprites/battle/furiell.png` + *Undertale — Toriel dialogue face sprites* → colar o prompt da seção 7. Detalhes de cada anexo na seção 6.

## 1. Contexto

Furiell (ver A3/B2): mãe sufocante e gentil, numa fursuit de cabra branca com óculos meia-lua e avental. A expressão *flustered* é o momento em que ela fecha uma aba do navegador muito rápido quando o Pepe entra (`* She closes a tab very fast.`).

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
| `assets/portraits/furiell_neutral.png` | sorriso suave, olhos semicerrados |
| `assets/portraits/furiell_happy.png` | olhos fechados em ^^, sorriso aberto |
| `assets/portraits/furiell_sad.png` | sobrancelhas para cima, boca curva para baixo |
| `assets/portraits/furiell_angry.png` | olhos estreitos, óculos com brilho, boca reta |
| `assets/portraits/furiell_flustered.png` | "fechou a aba": olhos arregalados, gota de suor, olhando de lado |

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
| md | Esta ficha | [`C01_portraits_furiell.md`](C01_portraits_furiell.md) | especificação completa |
| 1 | Template da grade/canvas | [`templates/C01_portraits_furiell.png`](templates/C01_portraits_furiell.png) | layout exato: proporção, linhas, colunas, pivô |
| 2 | Cartão de estilo | [`cards/C01_portraits_furiell.png`](cards/C01_portraits_furiell.png) | densidade de pixel (âncora), paleta, tamanho e regras numa imagem |
| 3 | Saída aprovada da ficha B2 | `assets/sprites/battle/furiell.png` (depois de aprovada) | mesmo personagem / mesmo design |
| 4 | Undertale — Toriel dialogue face sprites | The Spriters Resource → Undertale (só referência: salve em `assets/_ref/`, git-ignored) | enquadramento e expressões |

Referências são **só entrada**: material do Undertale ou de terceiros fica em `assets/_ref/` (git-ignored) e nunca é commitado nem copiado pixel a pixel.

## 7. Prompt (autocontido)

Cole inteiro. Ele repete as regras de estilo, a paleta e o formato, e explica cada anexo. Se não enviar alguma imagem, apague a linha dela em "Attached".

```text
You are drawing game art for nUndertale, a parody of the game Undertale. Rules: true pixel art in Undertale's style; every art pixel is one uniform square block; 1-pixel dark outline; flat colors with at most one shadow tone per color; no anti-aliasing, gradients, blur, painted texture or semi-transparent edges, including where the drawing meets the background. Match the pixel density of the anchor sprite on the style card: a human-sized overworld character is about 20x30 art pixels. Render the image large, but keep that number of art pixels. Use only the hex colors listed in this prompt. Adult proportions, nothing explicit, no real people, no brand logos, no text except exact words given here, no watermark or signature.

Attached: the spec file C01_portraits_furiell.md (full details) and these images, in order:
- image 1: canvas template: copy only its proportions and framing; never draw its gray boxes, guide lines or labels
- image 2: style card: match its pixel density (anchor sprite), palette, size and rules; never draw the card itself; the frog on it is only a pixel-density example, do not draw a frog or copy its features
- image 3: the approved design of the same character: keep its design exactly
- image 4: reference for framing and expressions only
Draw a new image; never copy the reference images.

Undertale battle sprite, monochrome white and black pixel art (only #FFFFFF and #000000): white 1-pixel line art, light materials such as fur, faces and costume heads filled solid white, dark materials such as clothes filled solid black, a continuous unbroken white 1-pixel outline around the ENTIRE outer edge of the silhouette (also around black clothes, hair, ears and tail, so no black part of the figure ever touches the background), a few long continuous white lines for folds and seams instead of scattered dots, the whole silhouette fully opaque, bold readable silhouette, hand-drawn pixel art, no anti-aliasing, no gray, no gradients, front-facing, centered, isolated on a solid flat #FF00FF magenta background, no watermark. Dialogue portrait sheet, 5 portraits in one row, identical framing in each (head and shoulders, 50x50 pixels each): a motherly woman in a fluffy white goat fursuit head with small horns, floppy ears and half-moon reading glasses, apron straps visible. Expressions left to right: gentle neutral smile, happy with closed ^^ eyes, sad with raised brows, stern angry with the glasses glinting, flustered wide-eyed with a sweat drop looking away as if caught closing a browser tab. No text.

Use only these colors: #FFFFFF (Traço), #000000 (Preenchimento interno).
Output: one image with 5 portraits side by side, framed like image 1, on a flat, exact #FF00FF magenta background with no noise.
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
