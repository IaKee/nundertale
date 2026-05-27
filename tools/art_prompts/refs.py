# External visual references per ficha, plus the style guide and project instructions documents.

SR = "The Spriters Resource → Undertale (só referência: salve em `assets/_ref/`, git-ignored)"
IMG = "busca de imagens"

def ut(what, use_pt, use_en):
    return (f"Undertale — {what}", SR, use_pt, use_en)

def photo(query, use_pt, use_en):
    return (f"Foto/ilustração real: {query}", f"{IMG}: \"{query}\"", use_pt, use_en)

LAYOUT_PT, LAYOUT_EN = "layout de 4 direções e ciclo de caminhada", "4-direction layout and walk cycle only"
LINE_PT, LINE_EN = "estilo do traço branco sobre preto", "white-on-black line style only"
BULLET_PT, BULLET_EN = "tamanho e leitura de um projétil do Undertale", "bullet size and readability only"
KENNEY = ("Kenney 1-Bit Pack", "kenney.nl (CC0)", "formas 1-bit legíveis em tamanho mínimo", "readable 1-bit shapes at tiny size")

REFS = {
    "A1": [("Sprite legado do Pepe", "`style/05_pepe_legacy.png` (do kit)", "identidade antiga: camiseta azul, cabeça larga", "legacy identity: blue T-shirt, wide head"),
           ("Pepe clássico de corpo inteiro (cara neutra, camiseta azul)", f"{IMG}: \"Pepe the Frog Boy's Club original\"", "traços do rosto: pálpebras, lábios, formato da cabeça", "face features only: eyelids, lips, head shape"),
           ut("Frisk overworld sprite sheet", LAYOUT_PT, LAYOUT_EN)],
    "A2": [ut("Frisk deitado no canteiro de flores (início do jogo)", "pose deitado e levantando", "lying and getting-up pose only")],
    "A3": [ut("Toriel overworld sprite sheet", "silhueta, altura e layout", "silhouette, height and layout only"),
           photo("white goat mascot costume head", "cabeça de fursuit de cabra", "goat costume head only"),
           photo("half-moon reading glasses", "óculos meia-lua", "half-moon glasses only"),
           photo("paw print apron", "avental com patinhas", "paw-print apron only")],
    "A4": [ut("Frisk overworld sprite sheet", LAYOUT_PT, LAYOUT_EN),
           photo("emo scene kid 2008 outfit", "franja emo, roupa", "emo fringe and outfit only"),
           photo("studded faux leather jacket", "jaqueta com tachas", "studded jacket only"),
           photo("wolf partial fursuit head", "orelhas e cabeça de lobo", "wolf ears and head only"),
           photo("superhero landing pose", "pose de aterrissagem (linha `drop`)", "landing pose only")],
    "A5": [ut("Flowey overworld sprites", "tamanho, caule e animação de afundar", "size, stem and sinking animation only"),
           photo("goat mascot costume head", "cabeça de mascote", "mascot head only"),
           photo("zipper close up", "dentes do zíper", "zipper teeth only")],
    "B1": [ut("Toriel battle sprite", LINE_PT, LINE_EN), photo("emo scene kid 2008 outfit", "franja e roupa", "fringe and outfit only"),
           photo("wolf partial fursuit head", "orelhas de lobo", "wolf ears only")],
    "B2": [ut("Toriel battle sprite", "pose, mangas e estilo do traço", "pose, sleeves and line style only"),
           photo("white goat mascot costume head", "cabeça de fursuit", "costume head only")],
    "B3": [ut("Flowey battle sprite", "tamanho e estilo do traço", "size and line style only"),
           photo("goat mascot costume head", "cabeça de mascote", "mascot head only")],
    "B4": [ut("Dummy battle sprite", "tamanho e estilo do traço", "size and line style only"),
           photo("training dummy wearing a mascot head", "a piada visual", "the visual gag only")],
    "B5": [ut("Froggit battle sprite", LINE_PT, LINE_EN), photo("homemade frog costume", "fantasia caseira de sapo", "homemade frog costume only")],
    "B6": [ut("Whimsun battle sprite", LINE_PT, LINE_EN), photo("cute moth illustration", "asas de mariposa", "moth wings only")],
    "B7": [ut("Moldsmal battle sprite", LINE_PT, LINE_EN), photo("jello mold dessert", "forma de gelatina", "jelly-mold shape only")],
    "B8": [ut("Loox battle sprite", LINE_PT, LINE_EN), photo("face lit by phone screen in the dark", "luz do celular", "phone glow only")],
    "B9": [ut("Migosp battle sprite", LINE_PT, LINE_EN), photo("well actually raised finger meme pose", "pose do dedo levantado", "raised-finger pose only")],
    "B10": [ut("Vegetoid battle sprite", LINE_PT, LINE_EN), photo("grass turf patch square", "tufo de grama", "grass clump only")],
    "C1": [ut("Toriel dialogue face sprites", "enquadramento e expressões", "framing and expressions only")],
    "C2": [ut("Flowey dialogue face sprites", "enquadramento e expressões", "framing and expressions only")],
    "C3": [ut("any enemy dialogue portrait", "enquadramento", "framing only"),
           photo("emo scene kid 2008 outfit", "franja e expressão emo", "emo fringe and expression only")],
    "D1": [ut("Flowey friendliness pellets", BULLET_PT, BULLET_EN), KENNEY],
    "D2": [ut("battle text-based attacks", BULLET_PT, BULLET_EN), photo("bubbly cute pixel font letters", "letras fofas", "letter style only")],
    "D3": [ut("any rotating bullet", BULLET_PT, BULLET_EN), photo("soda can pixel art", "forma da lata", "can shape only")],
    "D4": [ut("ring-shaped bullets", BULLET_PT, BULLET_EN), KENNEY],
    "D5": [ut("Toriel fireballs", BULLET_PT, BULLET_EN), photo("fluffy pom pom", "textura fofa", "fluffy edge only")],
    "D6": [ut("Toriel hand attack", "tamanho e movimento", "size and motion only"), photo("mascot costume paw glove", "pata de fantasia", "costume paw only")],
    "D7": [ut("Froggit attack bullets", BULLET_PT, BULLET_EN), KENNEY],
    "D8": [ut("Froggit fly bullets", BULLET_PT, BULLET_EN), KENNEY],
    "D9": [ut("Whimsun moth bullets", BULLET_PT, BULLET_EN), KENNEY],
    "D10": [ut("Moldsmal bullets", BULLET_PT, BULLET_EN), photo("ban hammer icon", "forma do martelo", "hammer shape only")],
    "D11": [ut("Migosp bug bullets", BULLET_PT, BULLET_EN), KENNEY],
    "D12": [ut("Vegetoid carrot bullets (green heal)", "convenção de verde = cura", "green-heals convention only"), KENNEY],
    "D13": [ut("Napstablook tear bullets", BULLET_PT, BULLET_EN), KENNEY],
    "E1": [ut("battle buttons (FIGHT/ACT/ITEM/MERCY)", "proporção, borda e posição do ícone", "proportions, border and icon position only"),
           photo("fps crosshair speech bubble chip bag white flag pixel icons", "ícones dos botões", "button icons only")],
    "E2": [ut("SOUL heart sprites", "formato exato do coração 8x8", "exact 8x8 heart shape only"), photo("plush heart with ears", "alma pelúcia", "plush heart only")],
    "E3": [ut("FIGHT target", "formato da elipse", "ellipse shape only"), photo("fps crosshair reticle", "retículo central", "center reticle only")],
    "E4": [ut("FIGHT attack bar", "tamanho e piscar", "size and flash only")],
    "E5": [ut("slash effect", "duração e escala", "duration and scale only"), photo("fps hitmarker", "o X do hitmarker", "hitmarker X only")],
    "E6": [ut("encounter exclamation bubble", "forma do balão", "bubble shape only")],
    "E7": [ut("GAME OVER logo", "peso e layout das letras", "letter weight and layout only"), photo("fur text effect", "borda felpuda", "fuzzy edge only")],
    "E8": [ut("title logo", "estilo das letras", "lettering style only")],
    "E9": [ut("title logo", "estilo da serifa", "serif style only")],
    "E10": [ut("save point star", "forma e giro da estrela", "star shape and spin only"), photo("green glass soda bottle", "garrafa", "bottle only")],
    "E11": [ut("battle HUD", "tamanho da barra de HP", "HP bar size only")],
    "E12": [ut("dialogue box", "espessura da borda", "border thickness only")],
    "E13": [ut("dialogue box with portrait", "moldura", "frame only")],
    "F1": [ut("Ruins tileset", "escala dos tiles, paredes, escadas e portas", "tile scale, walls, stairs and doors only"),
           photo("90s convention center carpet pattern", "padrão do carpete", "carpet pattern only"),
           photo("abandoned convention center hall", "clima de abandono", "abandoned mood only")],
    "F2": [photo("faded convention welcome banner", "o banner", "the banner only"), ut("Ruins signs", "escala", "scale only")],
    "F3": [photo("handmade paper sign masking tape", "a faixa improvisada", "the taped sign only")],
    "F4": [photo("convention vendor booth table canopy", "a tenda", "the booth only")],
    "F5": [photo("black velvet curtain on rod", "a cortina fechada", "the closed curtain only")],
    "F6": [photo("beige CRT monitor blue screen of death", "monitor e tela azul", "monitor and blue screen only")],
    "F7": [photo("cork notice board pinned papers", "o mural", "the board only")],
    "F8": [photo("gaming rgb light switch", "o interruptor", "the switch only"), ut("Ruins wall switches", "escala e leitura", "scale and readability only")],
    "F9": [ut("Ruins candy bowl (\"take one\")", "a mesma piada e escala", "same gag and scale only"), photo("bowl of tortilla chips", "os chips", "the chips only")],
    "F10": [photo("pile of crumpled chip bags", "as embalagens", "the wrappers only"), ut("Ruins red leaf pile", "escala e forma da pilha", "pile scale and shape only")],
    "F11": [ut("Ruins spikes", "tile ligado/desligado", "on/off floor tile only"), photo("tangled ethernet cables on floor", "os cabos", "the cables only")],
    "F12": [ut("Toriel's home fireplace", "escala e enquadramento", "scale and framing only"), photo("purple stone fireplace", "a lareira", "the fireplace only")],
    "F13": [ut("Toriel's reading chair", "escala", "scale only"), photo("armchair with faux fur throw", "a manta felpuda", "the fluffy throw only")],
    "F14": [ut("Ruins bedroom bed", "escala e vista", "scale and view only"), photo("faux fur blanket bed", "o cobertor", "the blanket only")],
    "F15": [ut("Ruins bedroom furniture", "escala", "scale only"), photo("wooden wardrobe open shelf", "o armário", "the wardrobe only")],
    "F16": [ut("Toriel's hallway mirror", "escala e moldura", "scale and frame only")],
    "F17": [ut("Toriel's bookshelf", "escala", "scale only"), photo("bookshelf faded books", "os livros", "the books only")],
    "F18": [photo("hanging fluorescent tube light fixture chains", "a luminária", "the fixture only")],
    "G1": [ut("intro story panels", "estilo e enquadramento", "style and framing only"),
           ("Card vizinho do projeto", "`assets/img/lore1.bmp`", "paleta sépia e acabamento", "sepia palette and finish only"),
           ("Card atual", "`assets/img/lore4.bmp`", "só a composição (tem rosto real e logo: não copiar)", "composition only; do not copy faces or logos")],
}

CHARACTERS = [
    ("Pepe", "Frisk", "cabeça larga de sapo com dois domos dos olhos", "pálpebras pesadas, dois lábios marrom-avermelhados, camiseta azul com manchas de nacho, chinelo", "A1, A2"),
    ("Furiell", "Toriel", "cabeçorra de cabra + avental", "fursuit branca fofa, óculos meia-lua, avental de patinhas, zíper nas costas", "A3, B2, C1"),
    ("Barky", "1º encontro / edgelord", "orelhas de lobo enormes", "franja emo com mecha verde, jaqueta com tachas, correntes de plástico, rabo com alfinete, zíper", "A4, B1, C3"),
    ("Floofy", "Flowey", "flor com cabeça de mascote e zíper", "pétalas amarelas, olhos de plástico, zíper no meio da cara; modo sinistro = escuridão + 2 olhos brancos", "A5, B3, C2"),
    ("Fursuit Dummy", "Dummy", "boneco + cabeça torta", "costuras, um pouco úmido", "B4"),
    ("Froggsona", "Froggit", "sapinho de fantasia caseira", "boca `:3`, sininho", "B5"),
    ("Shysona", "Whimsun", "asas de mariposa + caderno", "tímido, chorando", "B6"),
    ("Modsmal", "Moldsmal", "gelatina com distintivo MOD", "martelinho de ban", "B7"),
    ("Lurkeye", "Loox", "um olho gigante + celular", "olheiras, \"...\"", "B8"),
    ("Reply Guy", "Migosp", "inseto de óculos + dedo levantado", "\"well, actually\"", "B9"),
    ("Grasstoid", "Vegetoid", "tufo de grama pedindo abraço", "\"touch grass\"", "B10"),
]

def style_guide(palette_sections, kit_files):
    L = ["# nUndertale — Guia de identidade visual (Ruins)\n",
         "Documento de referência para o modelo que gera as imagens. Suba este arquivo e as imagens de `style/` no projeto do gerador; cada ficha (`A01_…md` etc.) assume que eles estão lá.\n",
         "## 1. Direção de arte\n",
         "nUndertale é uma paródia de Undertale: **a mesma linguagem de pixel art do Undertale** (sprites minúsculos desenhados a 1× e ampliados 2×, contorno de 1 px, cores chapadas), aplicada a uma piada sobre gamers e furries. O humor vem do conteúdo, nunca de um estilo diferente.\n",
         "**Pilares:**\n",
         "1. **Pixel de verdade** — cada pixel de arte é um bloco quadrado uniforme; sem anti-aliasing, gradiente, blur ou borda semitransparente.",
         "2. **Minúsculo** — um personagem do tamanho de uma pessoa tem ~20×30 pixels de arte. Mais detalhe que isso está errado.",
         "3. **Dois modos** — overworld colorido com paleta limitada por área; batalha em branco sobre preto.",
         "4. **Silhueta primeiro** — cada personagem tem um elemento que lê em preto sólido.",
         "5. **Paleta fechada** — só as cores hex das fichas e de `style/01_palettes.png`.\n",
         "## 2. Modos de render\n",
         "| Modo | Onde | Regras |\n| :--- | :--- | :--- |",
         "| Overworld | personagens andando, tiles, props | colorido, vista 3/4 de cima, contorno escuro de 1 px, 1 tom de sombra por cor |",
         "| Batalha | inimigos, bosses, retratos | traço `#FFFFFF`, interior **preto opaco** `#000000`, fora transparente; nada de cinza; cor só onde a ficha mandar |",
         "| Interface / projéteis | botões, almas, bullets, logos | formas sólidas e legíveis a 2×; projéteis brancos (verde `#00FF00` = cura) |",
         "| Cards de intro | `img/lore*` | monocromático sépia/laranja, imagem única |\n",
         "## 3. Escala e densidade\n",
         "- Tela nativa 320×240; o jogo desenha tudo a **2×** (640×480).",
         "- Overworld: Pepe 20×30 (célula 24×32), Barky 22×32, Furiell 26×40, Floofy 20×20, tile 20×20, save point 20×28.",
         "- Batalha: Barky 60×80, Furiell 80×110, Floofy 40×40, Dummy 40×60, comuns 30–60. A caixa de batalha tem 287 px de largura.",
         "- Ver `style/03_scale_lineup.png` e o sprite-âncora `style/04_pepe_anchor.png` (a densidade de referência).\n",
         "## 4. Paletas\n"]
    for title, items in palette_sections:
        L.append(f"**{title}:** " + " · ".join(f"{lab} `#{hx.strip().lstrip('#').upper()}`" for lab, hx in items) + "\n")
    L += ["## 5. Identidade dos personagens\n",
          "| Personagem | Paródia de | Silhueta (lê em preto) | Traços-chave | Fichas |\n| :--- | :--- | :--- | :--- | :--- |"]
    for c in CHARACTERS:
        L.append("| " + " | ".join(c) + " |")
    L += ["\nTodos são **adultos** (proporções adultas, nada infantil).\n",
          "## 6. O mundo das Ruins\n",
          "Os salões da **FurNation Con '14**, afundados há 12 anos: \"uma convenção furry abandonada de 2014 dentro das ruínas roxas do Undertale\". Pedra roxa rachada + carpete de convenção anos 90 (roxo com zigue-zague verde neon), banners lilás desbotados, tendas de lona, a Art Show com a cortina 18+ preta, monitores CRT com tela azul, cabos de rede, embalagens de salgadinho no chão. Partículas de \"fluff\" rosa e amarelo no ar (desenhadas pelo código, não nas imagens).\n",
          "## 7. Regras de sprite sheet\n",
          "- Células do mesmo tamanho, sem espaçamento; **uma linha por animação**, frames da esquerda para a direita a partir da **coluna 0**.",
          "- Estados/variantes (cores, cheio/vazio, normal/selecionado) vão em **linhas novas**.",
          "- Pé/base de todos os frames na mesma linha (pivô); células vazias transparentes.",
          "- Cada ficha tem um template em `templates/` com a grade exata.\n",
          "## 8. Faça / não faça\n",
          "| Faça | Não faça |\n| :--- | :--- |",
          "| contorno de 1 px, cores chapadas | anti-aliasing, gradiente, blur, textura pintada |",
          "| mesma densidade do sprite-âncora | personagens com 40+ px de altura no overworld |",
          "| preto opaco dentro dos sprites de batalha | interior transparente / cinza |",
          "| bordas 100% opacas | halo branco ou semitransparente |",
          "| texto em inglês, escrito à mão depois | texto inventado pela IA, marcas reais |",
          "| proporções adultas | rostos de bebê / chibi |",
          "| Pepe com os traços do meme (pálpebras, dois lábios, camiseta azul) | lábio laranja chato, olheira roxa |\n",
          "## 9. Conteúdo\n",
          "Nada explícito (o limite são blocos de mosaico, objetos fechados e linhas de fedor), nenhuma pessoa real, nenhum logo real, todo texto em inglês.\n",
          "## 10. Arquivos do kit\n",
          "| Arquivo | Para que serve |\n| :--- | :--- |"]
    for f, desc in kit_files:
        L.append(f"| `{f}` | {desc} |")
    return "\n".join(L) + "\n"

# Fichas whose approved output becomes the identity reference of another ficha.
DEPENDS = {
    "A2": "A1", "B1": "A4", "B2": "A3", "B3": "A5",
    "C1": "B2", "C2": "B3", "C3": "B1",
}

RULES_EN = ("You are drawing game art for nUndertale, a parody of the game Undertale. Rules: true pixel art in Undertale's style; "
            "every art pixel is one uniform square block; 1-pixel dark outline; flat colors with at most one shadow tone per color; "
            "no anti-aliasing, gradients, blur, painted texture or semi-transparent edges, including where the drawing meets the background. "
            "Match the pixel density of the anchor sprite on the style card: a human-sized overworld character is about 20x30 art pixels. "
            "Render the image large, but keep that number of art pixels. Use only the hex colors listed in this prompt. "
            "Adult proportions, nothing explicit, no real people, no brand logos, no text except exact words given here, no watermark or signature.")

def workflow(order_rows):
    L = ["# Como gerar cada imagem (sem projeto, até 5 anexos por chamada)\n",
         "Cada ficha (`A01_…md`, `B01_…md`…) tem no topo um bloco **Envio** com exatamente o que mandar numa conversa nova do gerador:\n",
         "1. **O arquivo `.md` da ficha** (especificação completa).",
         "2. **Até 4 imagens**, na ordem da ficha: o template da grade, o cartão de estilo da ficha (densidade de pixel + paleta + tamanho + regras numa imagem só) e 1–2 referências de identidade.",
         "3. **O prompt da seção 7**, colado inteiro. Ele é autocontido: repete as regras de estilo, a paleta, o formato e explica o papel de cada anexo.\n",
         "Se o seu gerador não contar o `.md` como referência, dá para trocar o `.md` pela 5ª imagem listada em \"Alternativas\" da ficha (o prompt já contém tudo do `.md` que importa para a imagem).\n",
         "## Dicas\n",
         "- Personagens: peça primeiro **só o frame parado de frente** (acrescente \"Only draw the front idle frame, alone, at the size of one cell\"), aprove e depois peça o sheet inteiro, trocando a última referência pelo frame aprovado.",
         "- Fundo: o gerador costuma entregar um magenta com ruído (`#EF0DF0` etc.). Recorte com tolerância (R>170, G<110, B>170) e limpe a franja rosada do contorno.",
         "- A saída sai grande (~9–10 px de tela por pixel de arte). Reduza com o pixel-âncora como medida, encaixe na paleta e retoque os perfis à mão.\n",
         "## Ordem recomendada (as saídas aprovadas viram referência de outras fichas)\n",
         "| Primeiro | Depois (usa a saída aprovada como referência) |\n| :--- | :--- |"]
    L += order_rows
    L += ["\n## Arquivos de apoio\n",
          "| Pasta | Conteúdo |\n| :--- | :--- |",
          "| `templates/` | grade/canvas exato de cada ficha (anexo 1) |",
          "| `cards/` | cartão de estilo de cada ficha (anexo 2) |",
          "| `style/` | imagens gerais (paletas, regras, escala, âncora, Pepe legado) — para consulta e como referência alternativa |",
          "| `STYLE_GUIDE.md` | guia de identidade visual completo (para você; não precisa enviar) |\n"]
    return "\n".join(L)
