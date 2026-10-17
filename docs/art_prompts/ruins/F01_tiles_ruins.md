# F1 · `assets/tiles/ruins.png` — Tileset das Ruins

> Ficha autocontida: tudo o que é preciso para gerar e entregar esta imagem. Índice e regras gerais em [README.md](README.md).

| Campo | Valor |
| :--- | :--- |
| Arquivo final | `assets/tiles/ruins.png` |
| Prioridade | P0 — Ruins · **núcleo da vertical slice** |
| Onde aparece | R1–R13. |
| O código já lê? | Não. Depende do loader de mapas do Tiled (OVW-003). |

## Envio (resumo)

Conversa nova no gerador → enviar **5 anexos**: `F01_tiles_ruins.md` + `templates/F01_tiles_ruins.png` + `cards/F01_tiles_ruins.png` + *Undertale — Ruins tileset* + *Foto/ilustração real: 90s convention center carpet pattern* → colar o prompt da seção 7. Detalhes de cada anexo na seção 6.

## 1. Contexto

**Conceito (LORE §12):** os salões originais da **FurNation Con '14**, afundados há 12 anos — "uma convenção furry abandonada de 2014 dentro das ruínas roxas do Undertale". Pedra roxa rachada misturada com carpete de convenção, banners desbotados, tendas de lona, a Art Show com a seção 18+ atrás de uma cortina preta. Embalagens de salgadinho no chão (crocantes).

Substitui o fundo atual do overworld (`assets/img/test.bmp`, que é o papel de parede "Bliss" do Windows XP — foto com copyright, não pode ir para build público).

## 2. Regras que valem para esta imagem

- **Escala:** arte em **1× nativo** (tela cheia = 320×240). O jogo desenha a **2×** com filtro *point* num canvas virtual de 640×480. Todos os tamanhos desta ficha são 1×.
- **Pixel:** sem anti-aliasing, sem gradiente suave, sem blur, sem sub-pixel.
- **Sprite sheet:** grade fixa, células do mesmo tamanho, sem espaçamento nem margem. **Uma linha por animação**, frames da esquerda para a direita **começando na coluna 0**. O loader não sabe começar em outra coluna, então **estados/variantes diferentes vão em linhas diferentes**. Célula vazia = transparente.
- **Pivô (`origin`):** o mesmo ponto (pé / base central) em todas as células.
- **Conteúdo:** nada explícito; proporções **adultas**; nenhum rosto real; nenhuma marca real (nada de logo Doritos / Mountain Dew; use "CITRUS BLAST", chips sem marca); todo texto dentro da arte em **inglês**.
- **Undertale:** pode servir de referência de tamanho/estilo, mas nada copiado vai para o repositório.
- Tiles de **20×20** em grade exata, chão emendando sem costura.
- Paleta das Ruins (abaixo), vista 3/4 de cima.

## 3. Formato de output esperado

- **Tipo:** PNG RGBA 32-bit, fundo transparente.
- **Célula (um frame):** 20×20 px.
- **Grade:** 8 coluna(s) × 8 linha(s).
- **Tamanho total do PNG:** **160×160 px** (1× nativo; o jogo desenha a 2×).
- Tileset para o **Tiled** (não usa o JSON de sprite).
- Tiles de chão emendam sem costura em todas as direções.
- ~55 dos 64 slots usados; slots livres ficam transparentes.
- **Do gerador de imagem:** peça em alta resolução com fundo `#FF00FF`; o arquivo entregue é o pequeno, já reduzido e recortado (ver "Do gerador ao arquivo final").

**Montagem do sheet:**

| Linha | Col 0–7 |
| :---: | :--- |
| 0 | pedra A · pedra B · pedra C · pedra rachada · pedra com glitter · embalagem amassada no chão · migalhas de chips · buraco/vazio preto |
| 1 | carpete de convenção (padrão anos 90, roxo com zigue-zague verde neon): canto sup-esq · topo · canto sup-dir · esquerda · centro · direita · carpete gasto · mancha |
| 2 | carpete: canto inf-esq · base · canto inf-dir · fita adesiva no chão · tapete de boas-vindas · pegada de pata · *(livre)* · *(livre)* |
| 3 | parede topo (borda): esq · meio · dir · parede frente: esq · meio · dir · frente rachada · frente com duto de ventilação |
| 4 | canto interno esq · canto interno dir · canto externo esq · canto externo dir · frente com gancho de banner · frente com tomada + cabo · sombra na base da parede · parede escura |
| 5 | escada: topo esq · topo meio · topo dir · meio esq · meio meio · meio dir · base esq · base dir |
| 6 | porta fechada (topo, base) · porta aberta (topo, base) · arco: sup-esq · sup-dir · inf-esq · inf-dir |
| 7 | pilar: topo · meio · base · placa neon de fundo · feixe de luz semitransparente (topo, base) · borda de desnível · *(livre)* |

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

Nenhuma utilizável (o `test.bmp` atual é a foto Bliss).

## 6. Pacote de envio (até 5 anexos)

Numa conversa nova, anexe **nesta ordem** e cole o prompt da seção 7:

| # | Anexo | Onde está | Para quê |
| :---: | :--- | :--- | :--- |
| md | Esta ficha | [`F01_tiles_ruins.md`](F01_tiles_ruins.md) | especificação completa |
| 1 | Template da grade/canvas | [`templates/F01_tiles_ruins.png`](templates/F01_tiles_ruins.png) | layout exato: proporção, linhas, colunas, pivô |
| 2 | Cartão de estilo | [`cards/F01_tiles_ruins.png`](cards/F01_tiles_ruins.png) | densidade de pixel (âncora), paleta, tamanho e regras numa imagem |
| 3 | Undertale — Ruins tileset | The Spriters Resource → Undertale (só referência: salve em `assets/_ref/`, git-ignored) | escala dos tiles, paredes, escadas e portas |
| 4 | Foto/ilustração real: 90s convention center carpet pattern | busca de imagens: "90s convention center carpet pattern" | padrão do carpete |

**Alternativas** (troque uma imagem acima, ou use como 5ª se o gerador não contar o `.md`):

- Foto/ilustração real: abandoned convention center hall — busca de imagens: "abandoned convention center hall" — clima de abandono

Referências são **só entrada**: material do Undertale ou de terceiros fica em `assets/_ref/` (git-ignored) e nunca é commitado nem copiado pixel a pixel.

## 7. Prompt (autocontido)

Cole inteiro. Ele repete as regras de estilo, a paleta e o formato, e explica cada anexo. Se não enviar alguma imagem, apague a linha dela em "Attached".

```text
You are drawing game art for nUndertale, a parody of the game Undertale. Rules: true pixel art in Undertale's style; every art pixel is one uniform square block; 1-pixel dark outline; flat colors with at most one shadow tone per color; no anti-aliasing, gradients, blur, painted texture or semi-transparent edges, including where the drawing meets the background. Match the pixel density of the anchor sprite on the style card: a human-sized overworld character is about 20x30 art pixels. Render the image large, but keep that number of art pixels. Use only the hex colors listed in this prompt. Adult proportions, nothing explicit, no real people, no brand logos, no text except exact words given here, no watermark or signature.

Attached: the spec file F01_tiles_ruins.md (full details) and these images, in order:
- image 1: layout template: copy its canvas proportions, rows, columns and cells exactly (one drawing per gray cell, white cells empty, each drawing centered in its cell); never draw its grid, gray cells, red marks or labels
- image 2: style card: match its pixel density (anchor sprite), palette, size and rules; never draw the card itself; the frog on it is only a pixel-density example, do not draw a frog or copy its features
- image 3: reference for tile scale, walls, stairs and doors only
- image 4: reference for carpet pattern only
Draw a new image; never copy the reference images.

Pixel art top-down RPG tileset in the style of Undertale's Ruins, 3/4 top-down view, crisp hard pixels, flat shading, no anti-aliasing, no gradients, no blur, limited palette, no watermark. Top-down RPG tileset, 20x20 pixel tiles on a strict grid of 8 columns by 8 rows: cracked purple stone floor variants, a 1990s convention carpet with a purple and neon green zigzag pattern with 9-slice edges, purple brick walls with top caps, front faces, inner and outer corners, a wall with a ventilation duct, a wall with a power outlet and a cable, stairs, doors closed and open, an archway, pillars, a pale light beam, scattered snack wrappers and chip crumbs on the floor. Palette strictly #3B1F5C, #5E3A87, #7CFF3A, #F4E3FF, #C9A0DC. An abandoned furry convention from 2014 inside ancient purple ruins. Seamless floor tiles, no text.

Use only these colors: #3B1F5C (Base escura), #5E3A87 (Base média), #7CFF3A (Acento neon), #F4E3FF (Realce), #C9A0DC (Banners), #111111 (Cortina 18+), #1E3FBF (Tela CRT), #4A2A70 (Roxos intermediários), #7A55A8 (Roxos intermediários), #9C7CC4 (Roxos intermediários).
Output: one image with the whole sheet laid out exactly like image 1 (8 columns x 8 rows of equal cells), on a flat, exact #FF00FF magenta background with no noise.
Avoid: blurry, anti-aliased, soft shading, smooth gradient, 3D render, realistic, photo, painterly, noisy texture, random dithering, jpeg artifacts, watermark, signature, brand logo, real person, real face, child proportions, baby face, nsfw, suggestive, cropped, cut off, inconsistent sizes between frames.
```

## 8. Do gerador ao arquivo final

1. Numa conversa nova do gerador, envie o pacote da seção 6 (o `.md` + até 4 imagens) e cole o prompt da seção 7.
2. Se o gerador errar a grade, gere **uma pose/frame por vez** (comece pelo parado de frente), aprove, e use o frame aprovado como referência para os outros; monte o sheet à mão. Alternativa: geradores nativos de pixel art (PixelLab, Retro Diffusion para Aseprite) já entregam no tamanho real.
3. No Aseprite / LibreSprite / Pixelorama: reduza para o tamanho nativo com **nearest neighbor** (ou redesenhe por cima), quantize para a paleta da seção 4, limpe o contorno para 1 px.
4. Monte na grade da seção 3, alinhe o pivô com uma guia, troque o magenta por transparência.
5. Exporte no formato da seção 3, escreva o JSON (seção "JSON", quando houver) e registre o arquivo em `assets/PLACEHOLDERS.md` (fonte/gerador, autor, licença).

**Específico desta imagem:**

- A IA não respeita grade de tiles: use o resultado como referência de textura e redesenhe/recorte cada tile em 20×20.
- Teste a emenda copiando cada tile de chão 3×3.

## 9. Checklist de aceite

- [ ] O PNG tem exatamente 160×160 px
- [ ] Sem anti-aliasing e sem cores fora da paleta da seção 4
- [ ] Fundo transparente (magenta removido)
- [ ] Chão sem costura visível.
- [ ] Só cores da paleta das Ruins.
- [ ] Registrado em `assets/PLACEHOLDERS.md` se não for a arte final
