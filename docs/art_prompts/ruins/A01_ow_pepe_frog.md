# A1 · `assets/sprites/ow/pepe_frog.png` — Pepe (protagonista) — overworld

> Ficha autocontida: tudo o que é preciso para gerar e entregar esta imagem. Índice e regras gerais em [README.md](README.md).

| Campo | Valor |
| :--- | :--- |
| Arquivo final | `assets/sprites/ow/pepe_frog.png` |
| JSON | `assets/sprites/ow/pepe_frog.json` |
| Prioridade | P0 — Ruins · **núcleo da vertical slice** |
| Onde aparece | Todas (é o sprite que o jogador controla). |
| O código já lê? | **Sim** — `OverworldScene` carrega `pepe_frog.png` + `pepe_frog.json` e cai no BMP legado se faltar algum. |

## Envio (resumo)

Conversa nova no gerador → enviar **5 anexos**: `A01_ow_pepe_frog.md` + `templates/A01_ow_pepe_frog.png` + `cards/A01_ow_pepe_frog.png` + `style/05_pepe_legacy.png` + *Pepe clássico de corpo inteiro (cara neutra, camiseta azul)* → colar o prompt da seção 7. Detalhes de cada anexo na seção 6.

## 1. Contexto

Pepe the Fallen Gamer, paródia da Frisk. 26 anos, mora no porão da mãe, vive de raids, gacha e salgadinho. Sarcástico, preguiçoso, diz odiar furries (secretamente gosta — o jogo nunca diz). **Nunca fala** em voz alta.

**Visual:** o sapo do meme — verde, cara cansada, **camiseta azul** com manchas de nacho, short, chinelo, **mãos verdes** (a mesma pele do rosto).

**Traços que fazem ele ser reconhecido como o Pepe** (o que faltou na 1ª geração):

| Traço | Como é | Erro a evitar |
| :--- | :--- | :--- |
| Cabeça | larga e meio irregular, **mais larga que os ombros**, ~45% da altura; dois domos redondos dos olhos **saltando acima** do topo | cabeça compacta, do tamanho do corpo |
| Olhos | grandes, brancos, **cortados ao meio por uma pálpebra verde pesada e caída**; pupila preta grande **apoiada na pálpebra de baixo**, olhar entediado de lado | olho aberto com íris pequena |
| Olheiras | 1 linha **verde-escura** sutil embaixo de cada olho | manchas roxas (leem como olho roxo) |
| Boca | lábios **muito largos** (quase a cara toda), **dois lábios** separados por uma linha escura, marrom-avermelhado, levemente caídos; de perfil **projetam para frente** | lábio único laranja e chato (lê como bico de pato) |
| Pele | verde-sapo médio, com sombra verde-escura | verde-limão muito saturado |
| Corpo | baixinho, meio barrigudo, braços soltos, ombros caídos | postura ereta |
| Roupa | **camiseta azul** gola redonda (a do meme / do sprite legado) | moletom com capuz (o Sans já usa moletom azul) |
| Mãos | **verdes**, a mesma pele do rosto | pontas laranja/amarelas (a 2× leem como luva e afastam do Pepe) |

**No tamanho real (20×30 px)** isso vira: cabeça ~22×15 (mais larga que o corpo de ~13 px), domos 2 px acima da linha da cabeça, cada olho 5×4 px (linha de cima = pálpebra, embaixo branco com a pupila de 2 px no canto de baixo), lábios ~18×4 px (contorno, lábio de cima, linha, lábio de baixo). O sprite-âncora `style/04_pepe_anchor.png` mostra exatamente isso.

**Decisão (2026-10-17):** o sprite segue o meme (traços do rosto + camiseta azul), substituindo o antigo "moletom cinza"; LORE §8.4/§10.1, ASSETS_NEEDED §4.1, BACKLOG e ROTEIRO já foram atualizados. O Pepe é um personagem com copyright do Matt Furie — vale reavaliar antes de um lançamento comercial.

**Decisão (2026-10-17, revisão):** as pontas dos dedos laranja saíram. Eram um aceno ao Undertale, mas na geração viraram mãos amarelas e o personagem deixou de ler como Pepe. A referência à Frisk fica no papel (protagonista calado), na pose dura de braços soltos e no layout de 4 direções do sheet, **não** em cor no corpo: a 20×30 px cada pixel colorido compete com os traços do meme.

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
- **Célula (um frame):** 24×32 px.
- **Grade:** 4 coluna(s) × 4 linha(s).
- **Tamanho total do PNG:** **96×128 px** (1× nativo; o jogo desenha a 2×).
- **Pivô (`origin`):** [12, 31] dentro de cada célula.
- Personagem ocupa ~22×31 dentro da célula (cabeça larga; corpo ~13 px).
- A linha 3 pode ser a linha 2 espelhada, mas **precisa existir no sheet** (o código não espelha).
- Peça ao gerador a mesma **densidade de pixels do sprite-âncora**, blocos uniformes e bordas 100% opacas (a 1ª geração saiu com ~25×37 px de arte, blocos irregulares e borda semitransparente).
- **Do gerador de imagem:** peça em alta resolução com fundo `#FF00FF`; o arquivo entregue é o pequeno, já reduzido e recortado (ver "Do gerador ao arquivo final").

**Montagem do sheet:**

| Linha | Animação | Col 0 | Col 1 | Col 2 | Col 3 |
| :---: | :--- | :--- | :--- | :--- | :--- |
| 0 | `walk_down` / `stand_down` | parado, de frente | passo pé esquerdo | parado (cópia da col 0) | passo pé direito |
| 1 | `walk_up` / `stand_up` | parado, de costas | passo esquerdo | parado (cópia) | passo direito |
| 2 | `walk_left` / `stand_left` | parado, perfil esquerdo | passo | *(vazio)* | *(vazio)* |
| 3 | `walk_right` / `stand_right` | parado, perfil direito | passo | *(vazio)* | *(vazio)* |

## 4. Paleta

| Uso | Hex |
| :--- | :--- |
| Contorno, linha entre os lábios | `#3D120E` |
| Pele / sombra | `#5FA33A` / `#3F7A2A` |
| Linha da olheira | `#2E5E22` |
| Olho branco / pupila | `#FFFFFF` / `#141414` |
| Lábio de cima / de baixo | `#C8553D` / `#A8432E` |
| Camiseta azul / sombra | `#2F62D6` / `#1F449E` |
| Manchas de nacho (só na camiseta) | `#F0A03A` |
| Short (do legado) / sombra | `#67A4E0` / `#4A80BC` |
| Chinelo | `#2B2B2B` |

## 5. Referência atual

`assets/img/pepe_char.bmp` (66×90, grade 3×3 de células 22×30, fundo magenta), cópia limpa em `style/05_pepe_legacy.png`: linha 0 de frente, linha 1 perfis, linha 2 de costas. É a melhor referência de identidade antiga (camiseta azul, cabeça larga, lábio grosso). O **sprite-âncora** `style/04_pepe_anchor.png` é a versão nova de frente, já com os traços do meme — o sheet tem que bater com ele.

## 6. Pacote de envio (até 5 anexos)

Numa conversa nova, anexe **nesta ordem** e cole o prompt da seção 7:

| # | Anexo | Onde está | Para quê |
| :---: | :--- | :--- | :--- |
| md | Esta ficha | [`A01_ow_pepe_frog.md`](A01_ow_pepe_frog.md) | especificação completa |
| 1 | Template da grade/canvas | [`templates/A01_ow_pepe_frog.png`](templates/A01_ow_pepe_frog.png) | layout exato: proporção, linhas, colunas, pivô |
| 2 | Cartão de estilo | [`cards/A01_ow_pepe_frog.png`](cards/A01_ow_pepe_frog.png) | densidade de pixel (âncora), paleta, tamanho e regras numa imagem |
| 3 | Sprite legado do Pepe | `style/05_pepe_legacy.png` (do kit) | identidade antiga: camiseta azul, cabeça larga |
| 4 | Pepe clássico de corpo inteiro (cara neutra, camiseta azul) | busca de imagens: "Pepe the Frog Boy's Club original" | traços do rosto: pálpebras, lábios, formato da cabeça |

**Alternativas** (troque uma imagem acima, ou use como 5ª se o gerador não contar o `.md`):

- Undertale — Frisk overworld sprite sheet — The Spriters Resource → Undertale (só referência: salve em `assets/_ref/`, git-ignored) — layout de 4 direções e ciclo de caminhada

Referências são **só entrada**: material do Undertale ou de terceiros fica em `assets/_ref/` (git-ignored) e nunca é commitado nem copiado pixel a pixel.

## 7. Prompt (autocontido)

Cole inteiro. Ele repete as regras de estilo, a paleta e o formato, e explica cada anexo. Se não enviar alguma imagem, apague a linha dela em "Attached".

```text
You are drawing game art for nUndertale, a parody of the game Undertale. Rules: true pixel art in Undertale's style; every art pixel is one uniform square block; 1-pixel dark outline; flat colors with at most one shadow tone per color; no anti-aliasing, gradients, blur, painted texture or semi-transparent edges, including where the drawing meets the background. Match the pixel density of the anchor sprite on the style card: a human-sized overworld character is about 20x30 art pixels. Render the image large, but keep that number of art pixels. Use only the hex colors listed in this prompt. Adult proportions, nothing explicit, no real people, no brand logos, no text except exact words given here, no watermark or signature.

Attached: the spec file A01_ow_pepe_frog.md (full details) and these images, in order:
- image 1: layout template: copy its canvas proportions, rows, columns and cells exactly (one drawing per gray cell, white cells empty, feet or base on the red line); never draw its grid, gray cells, red marks or labels
- image 2: style card: match its pixel density (anchor sprite), palette, size and rules; never draw the card itself
- image 3: reference for legacy identity: blue T-shirt, wide head
- image 4: reference for face features only: eyelids, lips, head shape
Draw a new image; never copy the reference images.

Pixel art sprite in the style of Undertale and Deltarune overworld sprites, 3/4 top-down RPG view, crisp 1-pixel dark outline, flat cel shading with one shadow tone per color, very limited palette, no anti-aliasing, no gradients, no blur, chunky readable pixels, adult proportions, isolated on a solid flat #FF00FF magenta background, no watermark. Character sprite sheet of a cartoon frog man drawn in the classic sad-frog internet comic look, a tired 26-year-old gamer; the front idle frame must match the anchor sprite exactly. Head: wide and lumpy, clearly wider than his shoulders, about 45% of body height, with two big round eye domes bulging above the top of the head. Eyes: large white eyes half covered by heavy droopy green eyelids (the lid line cuts each eye roughly in half), big black pupils resting on the lower lid, a bored half-lidded sideways stare, a faint darker-green line under each eye. Mouth: very wide thick two-part lips spanning almost the whole width of the face, reddish-brown upper lip #C8553D and darker lower lip #A8432E separated by a dark line, a slight frown, the lips protruding forward in profile. Skin: medium frog green #5FA33A with darker green #3F7A2A shading. Body: short and a little pudgy, plain blue crew-neck T-shirt #2F62D6 with one or two tiny orange nacho-cheese stains (small dots, not a logo), light blue shorts, black flip-flops #2B2B2B, plain green frog hands and legs in the same skin green as the face (no orange or yellow on the hands or feet), arms hanging loosely, slouched lazy posture. Strict grid of 4 rows, every frame the same size, feet on the same baseline: row 1 facing down (idle, left step, idle, right step), row 2 facing up showing the back of the head with the eye domes and the back of the T-shirt (idle, left step, idle, right step), row 3 facing left (idle, step), row 4 facing right (idle, step). Dark brown outline #3D120E. No text.

Use only these colors: #3D120E (Contorno, linha entre os lábios), #5FA33A (Pele), #3F7A2A (sombra), #2E5E22 (Linha da olheira), #FFFFFF (Olho branco), #141414 (pupila), #C8553D (Lábio de cima), #A8432E (de baixo), #2F62D6 (Camiseta azul), #1F449E (sombra), #F0A03A (Manchas de nacho), #67A4E0 (Short), #4A80BC (sombra), #2B2B2B (Chinelo).
Output: one image with the whole sheet laid out exactly like image 1 (4 columns x 4 rows of equal cells), on a flat, exact #FF00FF magenta background with no noise.
Avoid: blurry, anti-aliased, soft shading, smooth gradient, 3D render, realistic, photo, painterly, noisy texture, random dithering, jpeg artifacts, watermark, signature, brand logo, real person, real face, child proportions, baby face, nsfw, suggestive, cropped, cut off, inconsistent sizes between frames.
```

## 8. JSON

Salvar como `assets/sprites/ow/pepe_frog.json`. `frames` padrão 1, `fps` padrão 4, `loop` padrão `true`; animação sem loop congela no último frame; anim inexistente cai em `idle`.

```json
{
  "image": "pepe_frog.png",
  "cell": [24, 32],
  "origin": [12, 31],
  "anims": {
    "stand_down": { "row": 0, "frames": 1 },
    "walk_down": { "row": 0, "frames": 4, "fps": 6 },
    "stand_up": { "row": 1, "frames": 1 },
    "walk_up": { "row": 1, "frames": 4, "fps": 6 },
    "stand_left": { "row": 2, "frames": 1 },
    "walk_left": { "row": 2, "frames": 2, "fps": 4 },
    "stand_right": { "row": 3, "frames": 1 },
    "walk_right": { "row": 3, "frames": 2, "fps": 4 }
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

- Copie a cabeça da col 0 para todos os frames da linha e só mexa corpo/pés — lábio e olhos são o que mais "pula" entre frames.
- No frame de passo o corpo sobe 1 px.
- Confirme o pivô: pés tocando a linha y=31 em todas as células.
- **Lição da 1ª geração:** reduzir automaticamente o PNG grande para 24×32 borra os olhos, espalha pixels roxos e transforma o azul-marinho em preto. Use a imagem grande só como guia e **redesenhe a 1×** por cima, partindo do sprite-âncora.

## 10. Checklist de aceite

- [ ] O PNG tem exatamente 96×128 px
- [ ] Sem anti-aliasing e sem cores fora da paleta da seção 4
- [ ] Fundo transparente (magenta removido)
- [ ] JSON válido ao lado do PNG, com os nomes de animação da seção 3
- [ ] Andar nas 4 direções no jogo sem o sprite "tremer" (pivô fixo).
- [ ] Lê como o Pepe a 2× (pálpebras + dois lábios + cabeça larga).
- [ ] Frame parado de frente igual ao sprite-âncora.
- [ ] Camiseta azul, não moletom.
- [ ] Mãos verdes, sem laranja/amarelo; chinelo preto.
- [ ] Registrado em `assets/PLACEHOLDERS.md` se não for a arte final
