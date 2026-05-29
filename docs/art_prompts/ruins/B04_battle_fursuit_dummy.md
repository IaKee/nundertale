# B4 · `assets/sprites/battle/fursuit_dummy.png` — Fursuit Dummy — batalha (tutorial)

> Ficha autocontida: tudo o que é preciso para gerar e entregar esta imagem. Índice e regras gerais em [README.md](README.md).

| Campo | Valor |
| :--- | :--- |
| Arquivo final | `assets/sprites/battle/fursuit_dummy.png` |
| JSON | `assets/sprites/battle/fursuit_dummy.json` |
| Prioridade | P0 — Ruins |
| Onde aparece | R5 — tutorial de batalha (ensina Talk e Accept Hug). |
| O código já lê? | Não. Formato pronto para o loader. |

## Envio (resumo)

Conversa nova no gerador → enviar **5 anexos**: `B04_battle_fursuit_dummy.md` + `templates/B04_battle_fursuit_dummy.png` + `cards/B04_battle_fursuit_dummy.png` + *Undertale — Dummy battle sprite* + *Foto/ilustração real: training dummy wearing a mascot head* → colar o prompt da seção 7. Detalhes de cada anexo na seção 6.

## 1. Contexto

Paródia do Training Dummy. HP 15 · AT 0 · DF −5.
Check: `* FURSUIT DUMMY - ATK 0 DEF -5` / `* A mannequin wearing a fursuit head. Smells like the previous owner.`
Accept Hug: `* You hug the dummy. It's damp. FUR +10.` — Furiell: `You can always accept a hug, my child.`

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
- **Célula (um frame):** 40×60 px.
- **Grade:** 2 coluna(s) × 2 linha(s).
- **Tamanho total do PNG:** **80×120 px** (1× nativo; o jogo desenha a 2×).
- **Pivô (`origin`):** [20, 60] dentro de cada célula.
- **Do gerador de imagem:** peça em alta resolução com fundo `#FF00FF`; o arquivo entregue é o pequeno, já reduzido e recortado (ver "Do gerador ao arquivo final").

**Montagem do sheet:**

| Linha | Animação | Frames |
| :---: | :--- | :--- |
| 0 | `idle` | 2: parado / tecido da cabeça caído 1 px |
| 1 | `hurt` | 1: inclinado, enchimento saindo de uma costura |

## 4. Paleta

| Uso | Hex |
| :--- | :--- |
| Traço | `#FFFFFF` |
| Preenchimento interno (opaco) | `#000000` |

## 5. Referência atual

Nenhuma (o Dummy do Undertale serve de referência de tamanho).

## 6. Pacote de envio (até 5 anexos)

Numa conversa nova, anexe **nesta ordem** e cole o prompt da seção 7:

| # | Anexo | Onde está | Para quê |
| :---: | :--- | :--- | :--- |
| md | Esta ficha | [`B04_battle_fursuit_dummy.md`](B04_battle_fursuit_dummy.md) | especificação completa |
| 1 | Template da grade/canvas | [`templates/B04_battle_fursuit_dummy.png`](templates/B04_battle_fursuit_dummy.png) | layout exato: proporção, linhas, colunas, pivô |
| 2 | Cartão de estilo | [`cards/B04_battle_fursuit_dummy.png`](cards/B04_battle_fursuit_dummy.png) | densidade de pixel (âncora), paleta, tamanho e regras numa imagem |
| 3 | Undertale — Dummy battle sprite | The Spriters Resource → Undertale (só referência: salve em `assets/_ref/`, git-ignored) | tamanho e estilo do traço |
| 4 | Foto/ilustração real: training dummy wearing a mascot head | busca de imagens: "training dummy wearing a mascot head" | a piada visual |

Referências são **só entrada**: material do Undertale ou de terceiros fica em `assets/_ref/` (git-ignored) e nunca é commitado nem copiado pixel a pixel.

## 7. Prompt (autocontido)

Cole inteiro. Ele repete as regras de estilo, a paleta e o formato, e explica cada anexo. Se não enviar alguma imagem, apague a linha dela em "Attached".

```text
You are drawing game art for nUndertale, a parody of the game Undertale. Rules: true pixel art in Undertale's style; every art pixel is one uniform square block; 1-pixel dark outline; flat colors with at most one shadow tone per color; no anti-aliasing, gradients, blur, painted texture or semi-transparent edges, including where the drawing meets the background. Match the pixel density of the anchor sprite on the style card: a human-sized overworld character is about 20x30 art pixels. Render the image large, but keep that number of art pixels. Use only the hex colors listed in this prompt. Adult proportions, nothing explicit, no real people, no brand logos, no text except exact words given here, no watermark or signature.

Attached: the spec file B04_battle_fursuit_dummy.md (full details) and these images, in order:
- image 1: layout template: copy its canvas proportions, rows, columns and cells exactly (one drawing per gray cell, white cells empty, feet or base on the red line); never draw its grid, gray cells, red marks or labels
- image 2: style card: match its pixel density (anchor sprite), palette, size and rules; never draw the card itself; the frog on it is only a pixel-density example, do not draw a frog or copy its features
- image 3: reference for size and line style only
- image 4: reference for the visual gag only
Draw a new image; never copy the reference images.

Undertale battle sprite, monochrome white and black pixel art (only #FFFFFF and #000000): white 1-pixel line art, light materials such as fur, faces and costume heads filled solid white, dark materials such as clothes filled solid black, a continuous unbroken white 1-pixel outline around the ENTIRE outer edge of the silhouette (also around black clothes, hair, ears and tail, so no black part of the figure ever touches the background), a few long continuous white lines for folds and seams instead of scattered dots, the whole silhouette fully opaque, bold readable silhouette, hand-drawn pixel art, no anti-aliasing, no gray, no gradients, front-facing, centered, isolated on a solid flat #FF00FF magenta background, no watermark. Battle sprite of a stuffed cloth training dummy on a wooden post wearing a cheap, slightly crooked animal fursuit head with button eyes and droopy ears, visible stitches, looking a little damp. Front view, each frame in an equal 40x60 cell. Row 1: idle (2 frames, the head fabric sagging 1 pixel). Row 2: hurt, tilted with a bit of stuffing poking out of a seam. No text.

Use only these colors: #FFFFFF (Traço), #000000 (Preenchimento interno).
Output: one image with the whole sheet laid out exactly like image 1 (2 columns x 2 rows of equal cells), on a flat, exact #FF00FF magenta background with no noise.
Avoid: blurry, anti-aliased, soft shading, smooth gradient, 3D render, realistic, photo, painterly, noisy texture, random dithering, jpeg artifacts, watermark, signature, brand logo, real person, real face, child proportions, baby face, nsfw, suggestive, cropped, cut off, inconsistent sizes between frames.
```

## 8. JSON

Salvar como `assets/sprites/battle/fursuit_dummy.json`. `frames` padrão 1, `fps` padrão 4, `loop` padrão `true`; animação sem loop congela no último frame; anim inexistente cai em `idle`.

```json
{
  "image": "fursuit_dummy.png",
  "cell": [40, 60],
  "origin": [20, 60],
  "anims": {
    "idle": { "row": 0, "frames": 2, "fps": 1 },
    "hurt": { "row": 1, "frames": 1 }
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

- Costuras com pixels brancos tracejados.

## 10. Checklist de aceite

- [ ] O PNG tem exatamente 80×120 px
- [ ] Sem anti-aliasing e sem cores fora da paleta da seção 4
- [ ] Fundo transparente (magenta removido)
- [ ] JSON válido ao lado do PNG, com os nomes de animação da seção 3
- [ ] Lê como boneco de treino + cabeça de fursuit.
- [ ] Registrado em `assets/PLACEHOLDERS.md` se não for a arte final
