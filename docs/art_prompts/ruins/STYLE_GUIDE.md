# nUndertale — Guia de identidade visual (Ruins)

Documento de referência para o modelo que gera as imagens. Suba este arquivo e as imagens de `style/` no projeto do gerador; cada ficha (`A01_…md` etc.) assume que eles estão lá.

## 1. Direção de arte

nUndertale é uma paródia de Undertale: **a mesma linguagem de pixel art do Undertale** (sprites minúsculos desenhados a 1× e ampliados 2×, contorno de 1 px, cores chapadas), aplicada a uma piada sobre gamers e furries. O humor vem do conteúdo, nunca de um estilo diferente.

**Pilares:**

1. **Pixel de verdade** — cada pixel de arte é um bloco quadrado uniforme; sem anti-aliasing, gradiente, blur ou borda semitransparente.
2. **Minúsculo** — um personagem do tamanho de uma pessoa tem ~20×30 pixels de arte. Mais detalhe que isso está errado.
3. **Dois modos** — overworld colorido com paleta limitada por área; batalha em branco sobre preto.
4. **Silhueta primeiro** — cada personagem tem um elemento que lê em preto sólido.
5. **Paleta fechada** — só as cores hex das fichas e de `style/01_palettes.png`.

## 2. Modos de render

| Modo | Onde | Regras |
| :--- | :--- | :--- |
| Overworld | personagens andando, tiles, props | colorido, vista 3/4 de cima, contorno escuro de 1 px, 1 tom de sombra por cor |
| Batalha | inimigos, bosses, retratos | traço `#FFFFFF`, interior **preto opaco** `#000000`, fora transparente; **contorno branco fechado em volta da silhueta inteira** + keyline preta de 2 px por fora (§3.1); nada de cinza; cor só onde a ficha mandar |
| Interface / projéteis | botões, almas, bullets, logos | formas sólidas e legíveis a 2×; projéteis brancos (verde `#00FF00` = cura) |
| Cards de intro | `img/lore/*.png` | painel 200×110 desenhado a 2×, 6 tons de sépia, silhuetas chapadas (§3.2) |

## 3. Escala e densidade

- Tela nativa 320×240; o jogo desenha tudo a **2×** (640×480).
- Overworld: Pepe 20×30 (célula 24×32), Barky 22×32, Furiell 26×40, Floofy 20×20, tile 20×20, save point 20×28.
- Batalha: Barky 60×80, Furiell 80×110, Floofy 40×40, Dummy 40×60, comuns 30–60. A caixa de batalha tem 287 px de largura.
- Ver `style/03_scale_lineup.png` e o sprite-âncora `style/04_pepe_anchor.png` (a densidade de referência).

### 3.1 Batalha: como o Undertale fecha a silhueta

Medido em 2026-10-18 nos sprites de batalha originais (Toriel, Papyrus, Sans, Loox, Migosp, Whimsun, Vegetoid), em resolução nativa. O fundo da batalha é preto com uma grade verde, então um personagem de roupa preta só aparece se a borda for desenhada:

1. **Contorno branco fechado.** A borda externa de toda a silhueta é uma linha branca contínua de 1 px, inclusive onde o material é escuro (o manto da Toriel, a jaqueta do Sans). Nenhum preto do interior encosta no fundo.
2. **Keyline preta de 2 px por fora.** Em volta do contorno branco há 2 px de preto opaco antes do transparente (na Toriel, o branco encosta no transparente em 0 pixels). A grade verde para antes do personagem e nunca toca o branco.
3. **Interior preto com poucas linhas longas.** Dobras, costuras e bolsos são traços brancos contínuos de 1 px; nada de pontilhado nem pixels soltos. Materiais claros (pelo, crânio, cabeça de fantasia) são massas brancas com os detalhes em preto.
4. **Silhueta simples.** Poucas reentrâncias de 1 px; detalhe que não fecha em linha some a 2×.

O Barky v1 tinha 1374 pixels pretos na borda e 2 brancos: a jaqueta sumia no fundo. `tools/art_prompts/battle_outline.py` confere (`--check`: vazamentos e branco encostando no fundo) e corrige (contorno + keyline + limpeza) qualquer sheet de batalha; rode-o em toda saída do gerador.

### 3.2 Cards de intro: o painel do Undertale

Os painéis da intro do Undertale têm **200×110 pixels nativos desenhados a 2×** (400×220 na tela) e só **4–5 cores** de sépia. A cena é contada por **silhuetas chapadas**: fundo claro, figuras e primeiro plano escuros, sem contorno, sem gradiente, pouquíssimo detalhe. Nos nossos cards: PNG 200×110 em `assets/img/lore/`, paleta de 6 tons (Sépia 1–6; com só 5 o brilho do céu some na quantização), vermelho só no "BEGONE!". O jogo desenha o painel a 2× em (120, 62) e o texto embaixo.

## 4. Paletas

**RUINS - overworld area:** Base escura `#3B1F5C` · Base média `#5E3A87` · Acento neon `#7CFF3A` · Realce `#F4E3FF` · Banners `#C9A0DC` · Cortina 18+ `#111111` · Tela CRT `#1E3FBF` · Roxos intermediários `#4A2A70` · Roxos intermediários `#7A55A8` · Roxos intermediários `#9C7CC4`

**BATTLE - monochrome (+ blush, heal):** line `#FFFFFF` · fill `#000000` · blush `#FF77C8` · heal `#00FF00`

**UI - buttons:** normal `#FF7F27` · selected `#FFFF00` · locked `#808080`

**SOULS:** Vermelha `#FF0000` · Azul `#0026FF` · Verde `#00C000` · Amarela `#FFFF00` · Pelúcia `#FFB3E6` · costura `#FF77C8` · Sapo `#6FC418`

**PEPE:** Contorno, linha entre os lábios `#3D120E` · Pele `#5FA33A` · sombra `#3F7A2A` · Linha da olheira `#2E5E22` · Olho branco `#FFFFFF` · pupila `#141414` · Lábio de cima `#C8553D` · de baixo `#A8432E` · Camiseta azul `#2F62D6` · sombra `#1F449E` · Manchas de nacho `#F0A03A` · Short `#67A4E0` · sombra `#4A80BC` · Chinelo `#2B2B2B`

**FURIELL:** Contorno `#2A1640` · Pelo branco `#F4F0EA` · sombra lilás `#CFC3DD` · Chifres `#E6D8B8` · Avental `#C9A0DC` · estampa de patinhas `#5E3A87` · Vestido `#5E3A87` · Óculos `#E6B422` · Zíper `#9A9AAA` · Olhos `#3B1F5C`

**BARKY:** Contorno `#141418` · Pelo `#9A9AAA` · sombra `#6E6E7E` · Interior das orelhas `#C98AA8` · Jaqueta `#232329` · realce `#3C3C46` · Tachas, correntes plásticas `#C8C8D2` · Luvas `#232329` · Mecha emo na franja `#7CFF3A` · Olhos `#D0263A` · Alfinete / zíper `#9A9AAA`

**FLOOFY:** Contorno `#2A1A08` · Pétalas `#FFD23F` · sombra `#E6A800` · Caule e folhas `#3FA34D` · Caule e folhas `#2C7A38` · Cabeça de fursuit `#F4F0EA` · Cabeça de fursuit `#CFC3DD` · Olhos de plástico `#111111` · Zíper `#9A9AAA` · Interior aberto `#000000`

**SAVE POINT:** Estrela `#FFD23F` · brilho `#FFF6B0` · contorno `#8A5A00` · Garrafa `#3FA34D` · sombra `#2C7A38` · Rótulo `#7CFF3A` · Tampa `#E6E6E6`

**INTRO CARDS - sepia:** Sépia 1 `#2B1608` · Sépia 2 `#5A3416` · Sépia 3 `#8C5A24` · Sépia 4 `#B87D35` · Sépia 5 `#D9A04A` · Sépia 6 `#F0C878` · Sépia 7 `#FFE8B8` · Vermelho `#C0301A`

## 5. Identidade dos personagens

| Personagem | Paródia de | Silhueta (lê em preto) | Traços-chave | Fichas |
| :--- | :--- | :--- | :--- | :--- |
| Pepe | Frisk | cabeça larga de sapo com dois domos dos olhos | pálpebras pesadas, dois lábios marrom-avermelhados, camiseta azul com manchas de nacho, chinelo | A1, A2 |
| Furiell | Toriel | cabeçorra de cabra + avental | fursuit branca fofa, óculos meia-lua, avental de patinhas, zíper nas costas | A3, B2, C1 |
| Barky | 1º encontro / edgelord | orelhas de lobo enormes | franja emo com mecha verde, jaqueta com tachas, correntes de plástico, rabo com alfinete, zíper | A4, B1, C3 |
| Floofy | Flowey | flor com cabeça de mascote e zíper | pétalas amarelas, olhos de plástico, zíper no meio da cara; modo sinistro = escuridão + 2 olhos brancos | A5, B3, C2 |
| Fursuit Dummy | Dummy | boneco + cabeça torta | costuras, um pouco úmido | B4 |
| Froggsona | Froggit | sapinho de fantasia caseira | boca `:3`, sininho | B5 |
| Shysona | Whimsun | asas de mariposa + caderno | tímido, chorando | B6 |
| Modsmal | Moldsmal | gelatina com distintivo MOD | martelinho de ban | B7 |
| Lurkeye | Loox | um olho gigante + celular | olheiras, "..." | B8 |
| Reply Guy | Migosp | inseto de óculos + dedo levantado | "well, actually" | B9 |
| Grasstoid | Vegetoid | tufo de grama pedindo abraço | "touch grass" | B10 |

Todos são **adultos** (proporções adultas, nada infantil).

## 6. O mundo das Ruins

Os salões da **FurNation Con '14**, afundados há 12 anos: "uma convenção furry abandonada de 2014 dentro das ruínas roxas do Undertale". Pedra roxa rachada + carpete de convenção anos 90 (roxo com zigue-zague verde neon), banners lilás desbotados, tendas de lona, a Art Show com a cortina 18+ preta, monitores CRT com tela azul, cabos de rede, embalagens de salgadinho no chão. Partículas de "fluff" rosa e amarelo no ar (desenhadas pelo código, não nas imagens).

## 7. Regras de sprite sheet

- Células do mesmo tamanho, sem espaçamento; **uma linha por animação**, frames da esquerda para a direita a partir da **coluna 0**.
- Estados/variantes (cores, cheio/vazio, normal/selecionado) vão em **linhas novas**.
- Pé/base de todos os frames na mesma linha (pivô); células vazias transparentes.
- Cada ficha tem um template em `templates/` com a grade exata.

## 8. Faça / não faça

| Faça | Não faça |
| :--- | :--- |
| contorno de 1 px, cores chapadas | anti-aliasing, gradiente, blur, textura pintada |
| mesma densidade do sprite-âncora | personagens com 40+ px de altura no overworld |
| preto opaco dentro dos sprites de batalha | interior transparente / cinza |
| bordas 100% opacas | halo branco ou semitransparente |
| batalha: contorno branco fechado + 2 px de keyline preta | roupa preta encostando no fundo; contorno pontilhado |
| texto em inglês, escrito à mão depois | texto inventado pela IA, marcas reais |
| proporções adultas | rostos de bebê / chibi |
| Pepe com os traços do meme (pálpebras, dois lábios, camiseta azul) | lábio laranja chato, olheira roxa |

## 9. Conteúdo

Nada explícito (o limite são blocos de mosaico, objetos fechados e linhas de fedor), nenhuma pessoa real, nenhum logo real, todo texto em inglês.

## 10. Arquivos do kit

| Arquivo | Para que serve |
| :--- | :--- |
| `STYLE_GUIDE.md` | guia de identidade visual: direção de arte, modos, escala, paletas, personagens, regras |
| `WORKFLOW.md` | como montar cada chamada ao gerador (5 anexos) e em que ordem gerar |
| `style/01_palettes.png` | todas as paletas com hex |
| `style/02_rendering_rules.png` | regras de render com exemplos BOM / RUIM |
| `style/03_scale_lineup.png` | escala relativa de todos os personagens e objetos |
| `style/04_pepe_anchor.png` | sprite-âncora: densidade de pixels, contorno e sombreamento de referência |
| `style/04_pepe_anchor_native.png` | o mesmo sprite-âncora no tamanho real (24×32) |
| `style/05_pepe_legacy.png` | sprite antigo do Pepe (identidade) |
