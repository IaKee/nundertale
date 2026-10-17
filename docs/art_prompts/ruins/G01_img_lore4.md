# G1 · `assets/img/lore4.bmp` — Card de intro "BEGONE!" (redesenho obrigatório)

> Ficha autocontida: tudo o que é preciso para gerar e entregar esta imagem. Índice e regras gerais em [README.md](README.md).

| Campo | Valor |
| :--- | :--- |
| Arquivo final | `assets/img/lore4.bmp` |
| Prioridade | P0 — Ruins · **núcleo da vertical slice** |
| Onde aparece | Intro (antes do título). |
| O código já lê? | **Sim** — `LoreScene.cpp` carrega `assets/img/lore4.bmp` (usado nos cards 4 e 5). |

## Envio (resumo)

Conversa nova no gerador → enviar **5 anexos**: `G01_img_lore4.md` + `templates/G01_img_lore4.png` + `cards/G01_img_lore4.png` + *Undertale — intro story panels* + `assets/img/lore1.bmp` → colar o prompt da seção 7. Detalhes de cada anexo na seção 6.

## 1. Contexto

Card 4 da intro (propaganda gamer, narrador parcial). Texto que aparece com ele: `After a decisive ratio,` / `the GAMERS were victorious.` e, no card 5, `Seven GAMERS sealed the` / `degenerates underground` / `with a magic BARRIER.`

**Por que redesenhar (ART-170):** o card atual usa um **rosto real** (meme) e o **logo Doritos**.

**Composição (manter a do original):** metade esquerda, um **gamer demoníaco** gigante (silhueta com headset, olhos brilhando) rindo vitorioso, com um saco de salgadinho **sem marca** em primeiro plano; metade direita, **furries** em fursuit recuando/caindo; a palavra **"BEGONE!"** em diagonal cruzando o centro.

## 2. Regras que valem para esta imagem

- **Escala:** arte em **1× nativo** (tela cheia = 320×240). O jogo desenha a **2×** com filtro *point* num canvas virtual de 640×480. Todos os tamanhos desta ficha são 1×.
- **Pixel:** sem anti-aliasing, sem gradiente suave, sem blur, sem sub-pixel.
- **Sprite sheet:** grade fixa, células do mesmo tamanho, sem espaçamento nem margem. **Uma linha por animação**, frames da esquerda para a direita **começando na coluna 0**. O loader não sabe começar em outra coluna, então **estados/variantes diferentes vão em linhas diferentes**. Célula vazia = transparente.
- **Pivô (`origin`):** o mesmo ponto (pé / base central) em todas as células.
- **Conteúdo:** nada explícito; proporções **adultas**; nenhum rosto real; nenhuma marca real (nada de logo Doritos / Mountain Dew; use "CITRUS BLAST", chips sem marca); todo texto dentro da arte em **inglês**.
- **Undertale:** pode servir de referência de tamanho/estilo, mas nada copiado vai para o repositório.
- Card de intro: imagem única, sem transparência.
- Mesma linguagem visual dos outros cards (`lore1..9`): monocromático sépia/laranja.

## 3. Formato de output esperado

- **Tipo:** BMP 24-bit (sem alpha).
- **Tamanho:** 398×214 px (1× nativo; o jogo mostra a 796×428).
- **Sheet:** não — imagem única.
- **BMP 24-bit, sem alpha, 398×214** — troca direta, sem mexer no código.
- Alvo futuro (ART-041): PNG 400×220 para todos os cards; exige mudar o caminho em `LoreScene.cpp`.
- **Do gerador de imagem:** peça em alta resolução com fundo `#FF00FF`; o arquivo entregue é o pequeno, já reduzido e recortado (ver "Do gerador ao arquivo final").

## 4. Paleta

| Uso | Hex |
| :--- | :--- |
| Sépia 1 (mais escuro) | `#2B1608` |
| Sépia 2 | `#5A3416` |
| Sépia 3 | `#8C5A24` |
| Sépia 4 | `#B87D35` |
| Sépia 5 | `#D9A04A` |
| Sépia 6 | `#F0C878` |
| Sépia 7 (mais claro) | `#FFE8B8` |
| Vermelho (só no BEGONE!, opcional) | `#C0301A` |

## 5. Referência atual

`assets/img/lore4.bmp` (398×214, vermelho, com rosto real + logo — só para composição). Estilo dos vizinhos: `assets/img/lore1.bmp` (monocromático sépia/laranja).

## 6. Pacote de envio (até 5 anexos)

Numa conversa nova, anexe **nesta ordem** e cole o prompt da seção 7:

| # | Anexo | Onde está | Para quê |
| :---: | :--- | :--- | :--- |
| md | Esta ficha | [`G01_img_lore4.md`](G01_img_lore4.md) | especificação completa |
| 1 | Template da grade/canvas | [`templates/G01_img_lore4.png`](templates/G01_img_lore4.png) | layout exato: proporção, linhas, colunas, pivô |
| 2 | Cartão de estilo | [`cards/G01_img_lore4.png`](cards/G01_img_lore4.png) | densidade de pixel (âncora), paleta, tamanho e regras numa imagem |
| 3 | Undertale — intro story panels | The Spriters Resource → Undertale (só referência: salve em `assets/_ref/`, git-ignored) | estilo e enquadramento |
| 4 | Card vizinho do projeto | `assets/img/lore1.bmp` | paleta sépia e acabamento |

**Alternativas** (troque uma imagem acima, ou use como 5ª se o gerador não contar o `.md`):

- Card atual — `assets/img/lore4.bmp` — só a composição (tem rosto real e logo: não copiar)

Referências são **só entrada**: material do Undertale ou de terceiros fica em `assets/_ref/` (git-ignored) e nunca é commitado nem copiado pixel a pixel.

## 7. Prompt (autocontido)

Cole inteiro. Ele repete as regras de estilo, a paleta e o formato, e explica cada anexo. Se não enviar alguma imagem, apague a linha dela em "Attached".

```text
You are drawing game art for nUndertale, a parody of the game Undertale. Rules: true pixel art in Undertale's style; every art pixel is one uniform square block; 1-pixel dark outline; flat colors with at most one shadow tone per color; no anti-aliasing, gradients, blur, painted texture or semi-transparent edges, including where the drawing meets the background. Match the pixel density of the anchor sprite on the style card: a human-sized overworld character is about 20x30 art pixels. Render the image large, but keep that number of art pixels. Use only the hex colors listed in this prompt. Adult proportions, nothing explicit, no real people, no brand logos, no text except exact words given here, no watermark or signature.

Attached: the spec file G01_img_lore4.md (full details) and these images, in order:
- image 1: canvas template: copy only its proportions and framing; never draw its gray boxes, guide lines or labels
- image 2: style card: match its pixel density (anchor sprite), palette, size and rules; never draw the card itself; the frog on it is only a pixel-density example, do not draw a frog or copy its features
- image 3: reference for style and framing only
- image 4: reference for sepia palette and finish only
Draw a new image; never copy the reference images.

Pixel art story card in the style of Undertale's opening cutscene, limited palette, soft vignette, crisp pixels, no anti-aliasing, no gradients beyond the palette steps, no watermark. Pixel art intro story card, 398x214 pixels, limited monochrome sepia-orange palette (#2B1608, #5A3416, #8C5A24, #B87D35, #D9A04A, #F0C878, #FFE8B8). Left half: a towering triumphant cartoon gamer silhouette wearing a headset, glowing eyes, laughing with his mouth wide open, an unbranded bag of chips in the foreground. Right half: a crowd of cartoon furry characters in fursuits recoiling and tumbling backwards. Diagonally across the center, the word "BEGONE!" in big jagged white letters. Original characters only, no real faces, no brand logos, nothing explicit.

Use only these colors: #2B1608 (Sépia 1), #5A3416 (Sépia 2), #8C5A24 (Sépia 3), #B87D35 (Sépia 4), #D9A04A (Sépia 5), #F0C878 (Sépia 6), #FFE8B8 (Sépia 7), #C0301A (Vermelho).
Output: one image with aspect ratio 398:214, on a full scene with no transparency.
Avoid: blurry, anti-aliased, soft shading, smooth gradient, 3D render, realistic, photo, painterly, noisy texture, random dithering, jpeg artifacts, watermark, signature, brand logo, real person, real face, child proportions, baby face, nsfw, suggestive, cropped, cut off, inconsistent sizes between frames.
```

## 8. Do gerador ao arquivo final

1. Numa conversa nova do gerador, envie o pacote da seção 6 (o `.md` + até 4 imagens) e cole o prompt da seção 7.
2. Se o gerador errar a grade, gere **uma pose/frame por vez** (comece pelo parado de frente), aprove, e use o frame aprovado como referência para os outros; monte o sheet à mão. Alternativa: geradores nativos de pixel art (PixelLab, Retro Diffusion para Aseprite) já entregam no tamanho real.
3. No Aseprite / LibreSprite / Pixelorama: reduza para o tamanho nativo com **nearest neighbor** (ou redesenhe por cima), quantize para a paleta da seção 4, limpe o contorno para 1 px.
4. Monte na grade da seção 3, alinhe o pivô com uma guia, troque o magenta por transparência.
5. Exporte no formato da seção 3, escreva o JSON (seção "JSON", quando houver) e registre o arquivo em `assets/PLACEHOLDERS.md` (fonte/gerador, autor, licença).

**Específico desta imagem:**

- Quantize para a paleta.
- Escreva o "BEGONE!" à mão se a IA errar.
- Exporte BMP 24-bit sem alpha.

## 9. Checklist de aceite

- [ ] O arquivo tem exatamente 398×214 px
- [ ] Sem anti-aliasing e sem cores fora da paleta da seção 4
- [ ] Sem canal alpha (BMP 24-bit)
- [ ] Nenhum rosto real nem logo.
- [ ] Combina em tom com `lore1`–`lore9`.
- [ ] Registrado em `assets/PLACEHOLDERS.md` se não for a arte final
