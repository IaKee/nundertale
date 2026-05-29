# A04 · Barky overworld — geração por macro-pixels 8×

Este documento registra a tentativa realizada em 17 de outubro de 2026 para gerar novamente
`assets/sprites/ow/barky.png` usando o gerador de imagens do ChatGPT sem pedir que o modelo
desenhasse diretamente no canvas nativo muito pequeno.

A especificação visual e funcional continua sendo
[`A04_ow_barky.md`](A04_ow_barky.md). Este arquivo documenta apenas o processo experimental de
geração, mapeamento e validação. O resultado foi salvo em `assets/_gen/`; o sprite usado pelo jogo
não foi sobrescrito.

## 1. Objetivo

O Barky usa um sprite sheet nativo de apenas **112×180 px**, dividido em células de **28×36 px**.
Geradores de imagem tendem a ignorar uma resolução tão pequena ou a produzir uma ilustração grande
e detalhada que perde legibilidade quando reduzida.

A solução testada foi representar cada pixel nativo como um bloco grande e alinhado:

- escala macro: **8×**;
- um pixel nativo: **um bloco uniforme de 8×8 px**;
- sheet nativo: **112×180 px**;
- área macro equivalente: **896×1440 px**;
- canvas enviado ao gerador: **1024×1536 px**;
- posição da área macro dentro do canvas: **x=64, y=48**;
- margens: 64 px à esquerda/direita e 48 px acima/abaixo.

O resultado do gerador é convertido de volta para 112×180 por votação de cor em cada bloco 8×8.
Não há interpolação entre pixels, filtro bilinear ou redução fotográfica convencional.

## 2. Cálculo das dimensões

O sheet da ficha A04 tem:

```text
112 × 180 pixels nativos
4 colunas × 5 linhas
célula nativa: 28 × 36 pixels
```

Com escala inteira de 8×:

```text
sheet:   112×8 × 180×8 = 896 × 1440
célula:   28×8 ×  36×8 = 224 × 288
pixel:     1×8 ×   1×8 =   8 × 8
```

A área 896×1440 foi centralizada num canvas 1024×1536:

```text
(1024 - 896) / 2 = 64 px de margem horizontal
(1536 - 1440) / 2 = 48 px de margem vertical
```

Portanto, a área útil ocupa:

```text
x = 64..959
y = 48..1487
largura = 896
altura = 1440
```

O primeiro macro-pixel começa exatamente em `(64, 48)`. Todos os blocos seguintes são alinhados a
essa origem em passos de 8 px.

## 3. Especificação nativa preservada

### Formato

- PNG RGBA;
- 112×180 px;
- 4 colunas × 5 linhas;
- célula de 28×36 px;
- pivô lógico `[14, 35]` por célula;
- personagem com aproximadamente 22×32 pixels nativos;
- fundo transparente no arquivo final;
- `#FF00FF` usado como chroma key durante a geração.

### Layout das animações

| Linha | Animação | Coluna 0 | Coluna 1 | Coluna 2 | Coluna 3 |
| :---: | :--- | :--- | :--- | :--- | :--- |
| 0 | `walk_down` / `stand_down` | parado | passo esquerdo | cópia exata da col. 0 | passo direito |
| 1 | `walk_up` / `stand_up` | parado | passo esquerdo | cópia exata da col. 0 | passo direito |
| 2 | `walk_left` / `stand_left` | parado | passo | vazio | vazio |
| 3 | `walk_right` / `stand_right` | parado | passo | vazio | vazio |
| 4 | `drop` | caindo | aterrissando | levantando | vazio |

### Paleta permitida

| Uso | Cor |
| :--- | :--- |
| Contorno | `#141418` |
| Pelo | `#9A9AAA` |
| Sombra do pelo | `#6E6E7E` |
| Interior das orelhas | `#C98AA8` |
| Jaqueta e luvas | `#232329` |
| Realce da jaqueta | `#3C3C46` |
| Tachas e correntes | `#C8C8D2` |
| Mecha da franja | `#7CFF3A` |
| Olhos/delineador | `#D0263A` |
| Chroma key temporário | `#FF00FF` |

No PNG nativo, `#FF00FF` é convertido para transparência e não permanece na paleta final.

## 4. Entradas usadas

O modo integrado do ImageGen foi usado, sem chamada direta à API e sem `OPENAI_API_KEY`.

| Ordem | Arquivo | Papel na geração |
| :---: | :--- | :--- |
| 1 | `assets/_gen/ruins/A04_ow_barky-macro8-guide.png` | alvo de edição e geometria exata do canvas macro-pixel |
| 2 | `docs/art_prompts/ruins/cards/A04_ow_barky.png` | paleta, densidade, escala e regras visuais |
| 3 | `docs/art_prompts/ruins/templates/A04_ow_barky.png` | distribuição das animações na grade 4×5 |
| 4 | `docs/art_prompts/ruins/style/03_scale_lineup.png` | escala relativa do Barky no overworld |

### Como o guia macro foi criado

O guia partiu do sprite existente `assets/sprites/ow/barky.png`, usado apenas como referência de
identidade, poses e ocupação das células.

1. O PNG nativo 112×180 foi aberto como RGBA.
2. Pixels transparentes foram substituídos por `#FF00FF`.
3. O sheet foi ampliado para 896×1440 com **nearest-neighbor** e fator inteiro 8.
4. A imagem foi colada em `(64, 48)` num canvas 1024×1536 preenchido com `#FF00FF`.
5. O resultado foi salvo como `A04_ow_barky-macro8-guide.png`.

Essa ampliação não inventa pixels: cada pixel original vira exatamente um retângulo uniforme de
8×8 pixels.

## 5. Prompt integral usado no ImageGen

```text
Use case: precise-object-edit
Asset type: 8x macro-pixel overworld sprite sheet for direct deterministic decoding to native game pixels.

Input images:
- Image 1 is the EDIT TARGET and exact macro-pixel canvas. Preserve its exact 1024x1536 dimensions, magenta margins, sprite positions, 4x5 cell layout, occupied/empty cells, and animation poses.
- Image 2 is the Barky style card: use its restricted palette, native pixel density, hard edges, and 3/4 top-down style. Do not reproduce the card or frog.
- Image 3 documents the exact frame layout. Do not reproduce its labels, grid, lines, or boxes.
- Image 4 documents native character scale. Do not reproduce its labels or examples.

Primary request: redraw and improve Barky inside Image 1 while treating every visible 8x8 square as one indivisible macro-pixel. Every edge must follow the same global 8-pixel grid. Every 8x8 block must contain exactly one flat color with no texture, shading variation, antialiasing, blur, partial coverage, or smaller details. This is not ordinary high-resolution pixel art: it is a 112x180 sprite sheet represented at exactly 8x scale.

Canvas geometry:
- output canvas exactly 1024x1536
- useful sprite region from x=64 through x=959 and y=48 through y=1487, exactly 896x1440
- macro-pixel grid origin at x=64, y=48
- each native pixel is exactly one solid 8x8 block
- decoded native sheet is 112x180
- each cell is 224x288 in this image, representing 28x36 native pixels
- 4 columns x 5 rows
- outside useful region remains exact flat #FF00FF
- all transparent/native-empty pixels are represented by exact flat #FF00FF

Subject: Barky, an adult 23-year-old man in a cheap gray wolf fursuit. Huge pointed wolf ears, emo side-swept fringe with one neon-green streak, red eyeliner, black studded faux-leather jacket, plastic silver chains, fingerless gloves, fluffy wolf tail attached with a large safety pin, visible costume zipper on rear views. Dramatic try-hard posture. Preserve the clear character identity and poses of Image 1, but improve silhouette readability and animation consistency.

Exact layout:
row 1: facing down — idle, left step, exact visual duplicate of idle, right step.
row 2: facing up — idle showing zipper and safety-pinned tail, left step, exact visual duplicate of idle, right step.
row 3: facing left — idle, step, then two completely empty magenta cells.
row 4: facing right — idle, step, then two completely empty magenta cells.
row 5: falling with arms raised, superhero landing crouch with one fist down, rising into dramatic pose, then one completely empty magenta cell.

Palette only: #141418, #9A9AAA, #6E6E7E, #C98AA8, #232329, #3C3C46, #C8C8D2, #7CFF3A, #D0263A, plus #FF00FF for empty/background.

Constraints: change only Barky's pixel design within the already occupied cells. Keep the exact canvas geometry and macro-pixel alignment. One color per 8x8 block. Bold silhouette, approximately 22x32 native pixels per character. Feet aligned to each cell baseline. No grid lines, labels, text, watermark, extra characters, frog, decorations, shadows on the background, gradients, blur, antialiasing, glow, noise, dithering, semitransparency, or colors outside the palette.
```

## 6. Saída bruta do ImageGen

O gerador entregou um PNG 1024×1536. O arquivo recebido foi copiado para o projeto antes do
pós-processamento.

| Arquivo | Conteúdo |
| :--- | :--- |
| `assets/_gen/ruins/A04_ow_barky-macro8-source.png` | cópia direta da saída do ImageGen |
| `assets/_gen/ruins/A04_ow_barky-macro8-generated.png` | cópia preservada pelo pipeline no estágio de decodificação |

O modelo respeitou a estrutura geral, as 15 poses e as células vazias, mas não obedeceu
perfeitamente à regra de uma única cor por bloco 8×8. Alguns blocos continham pequenas variações,
bordas internas ou cores aproximadas. Por isso, foi necessária uma decodificação determinística.

## 7. Mapeamento macro-pixel → pixel nativo

O processamento está implementado em:

```text
tools/art_prompts/barky_macro_pipeline.py
```

### 7.1 Recorte da área útil

Da imagem 1024×1536, o pipeline recorta:

```text
left   = 64
top    = 48
right  = 64 + 896 = 960   (limite exclusivo)
bottom = 48 + 1440 = 1488 (limite exclusivo)
```

O resultado intermediário tem exatamente 896×1440 px.

### 7.2 Quantização para a paleta

Cada pixel RGB da área útil é classificado pela menor distância euclidiana quadrática até uma das
dez cores candidatas:

- as nove cores finais da ficha A04;
- `#FF00FF`, representando transparência.

Para uma cor de entrada `p` e uma cor candidata `c`, a métrica usada é:

```text
d(p, c) = (p.r - c.r)² + (p.g - c.g)² + (p.b - c.b)²
```

Isso remove cores aproximadas e pequenas variações criadas pelo modelo.

### 7.3 Votação dentro de cada bloco 8×8

Para cada coordenada nativa `(px, py)`:

```text
x0 = px × 8
x1 = (px + 1) × 8
y0 = py × 8
y1 = (py + 1) × 8
bloco = imagem_quantizada[y0:y1, x0:x1]
```

O pipeline cria um histograma das 64 classificações do bloco e seleciona a cor mais frequente:

```text
pixel_nativo(px, py) = moda(bloco_8x8)
```

Consequências importantes:

- nenhum pixel nativo mistura cores vizinhas;
- não há média de cores;
- não há filtro bilinear, bicúbico ou Lanczos;
- detalhes menores que o macro-pixel são descartados por votação;
- cada bloco produz exatamente um pixel final.

### 7.4 Invariantes corrigidas depois da votação

Algumas regras estruturais são impostas pelo pipeline em vez de depender do modelo:

1. Linha 0, coluna 2 recebe uma cópia binária exata da linha 0, coluna 0.
2. Linha 1, coluna 2 recebe uma cópia binária exata da linha 1, coluna 0.
3. As células não utilizadas são zeradas para transparência:
   - `(col=2, row=2)`;
   - `(col=3, row=2)`;
   - `(col=2, row=3)`;
   - `(col=3, row=3)`;
   - `(col=3, row=4)`.

### 7.5 Conversão do chroma key em alpha

- índices correspondentes às nove cores da ficha recebem alpha 255;
- índices correspondentes a `#FF00FF` recebem RGBA `(0, 0, 0, 0)`;
- o resultado final é um PNG RGBA com fundo transparente.

## 8. Arquivos produzidos

| Arquivo | Dimensão | Finalidade |
| :--- | :---: | :--- |
| `assets/_gen/ruins/A04_ow_barky-macro8-guide.png` | 1024×1536 | entrada macro-pixel enviada ao gerador |
| `assets/_gen/ruins/A04_ow_barky-macro8-source.png` | 1024×1536 | saída original copiada do ImageGen |
| `assets/_gen/ruins/A04_ow_barky-macro8-generated.png` | 1024×1536 | saída bruta preservada pelo pipeline |
| `assets/_gen/ruins/A04_ow_barky-macro8-native.png` | 112×180 | resultado nativo quantizado e transparente |
| `assets/_gen/ruins/A04_ow_barky-macro8-before-after.png` | 968×792 | comparação ampliada em nearest-neighbor |

O candidato a sprite ingame é:

```text
assets/_gen/ruins/A04_ow_barky-macro8-native.png
```

Ele ainda não substitui `assets/sprites/ow/barky.png`.

## 9. Como reproduzir

Na raiz do repositório, com Python, Pillow e NumPy disponíveis:

### Criar o guia macro-pixel

```powershell
python tools/art_prompts/barky_macro_pipeline.py prepare
```

Enviar o guia e as três referências listadas na seção 4 ao ImageGen, usando integralmente o prompt
da seção 5. Salvar a saída como:

```text
assets/_gen/ruins/A04_ow_barky-macro8-source.png
```

### Decodificar a saída

```powershell
python tools/art_prompts/barky_macro_pipeline.py decode assets/_gen/ruins/A04_ow_barky-macro8-source.png
```

O decoder rejeita a imagem se ela não tiver exatamente 1024×1536.

## 10. Validações executadas

O pipeline validou automaticamente:

- resultado final com exatamente 112×180 px;
- arquivo RGBA;
- somente cores pertencentes à paleta A04 nos pixels opacos;
- nove cores da paleta usadas nesta execução;
- frames idle duplicados exatamente;
- células não utilizadas transparentes;
- saída original do jogo não sobrescrita.

Resultado reportado pelo pipeline:

```text
validated: 112x180, 9 colors
```

## 11. Resultado observado

Comparado ao sprite anterior, o resultado macro-pixel apresentou:

- silhueta e cabeça mais consistentes entre frames;
- roupa, zíper, cauda e franja mais legíveis;
- melhor consistência visual entre frente, costas e perfis;
- poses da queda mais distintas;
- pixels finais rígidos e restritos à paleta.

A comparação visual está em:

```text
assets/_gen/ruins/A04_ow_barky-macro8-before-after.png
```

## 12. Limitações e cuidados

1. **O ImageGen não garantiu blocos 8×8 perfeitamente uniformes.** A votação por bloco foi
   necessária.
2. **O modelo pode deslocar detalhes dentro de um bloco.** Tudo que não dominar a maioria dos 64
   pixels desaparece na conversão.
3. **A grade precisa permanecer alinhada.** Mudar `OFFSET`, `SCALE` ou redimensionar a saída antes
   da decodificação altera o mapeamento.
4. **Não use compressão JPEG.** Artefatos podem mudar a classificação das cores; use PNG.
5. **A saída precisa ser 1024×1536.** Uma resolução diferente deve ser rejeitada, não esticada.
6. **Frames duplicados e células vazias não devem depender do modelo.** O pipeline os normaliza.
7. **Aprovação artística ainda é manual.** As validações garantem formato e paleta, não que cada pose
   esteja visualmente perfeita.

## 13. Possíveis melhorias

- adicionar validação automática do pivô/baseline em cada célula;
- medir o bounding box de cada frame e alertar diferenças grandes de escala;
- gerar frente, costas, perfis e `drop` em passes separados se a consistência de uma folha completa
  não for suficiente;
- fornecer ao ImageGen um guia macro em branco com máscaras de ocupação, para permitir mudanças
  maiores no design sem herdar tantos detalhes do sprite anterior;
- produzir duas ou três variações e selecionar a melhor antes da decodificação;
- adicionar uma etapa opcional de revisão manual no Aseprite mantendo a paleta e a grade nativas.

