# Generates docs/art_prompts/ruins/*.md : one self-contained prompt file per Ruins image.
# Run from the repo root: python tools/art_prompts/gen.py [out_dir]
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import kit
from refs import REFS, DEPENDS, RULES_EN, style_guide, workflow

OUT = sys.argv[1] if len(sys.argv) > 1 else os.path.join("docs", "art_prompts", "ruins")
for sub in ("", "style", "templates", "cards"):
    os.makedirs(os.path.join(OUT, sub), exist_ok=True)

KIT_FILES = [
    ("STYLE_GUIDE.md", "guia de identidade visual: direção de arte, modos, escala, paletas, personagens, regras"),
    ("WORKFLOW.md", "como montar cada chamada ao gerador (5 anexos) e em que ordem gerar"),
    ("style/01_palettes.png", "todas as paletas com hex"),
    ("style/02_rendering_rules.png", "regras de render com exemplos BOM / RUIM"),
    ("style/03_scale_lineup.png", "escala relativa de todos os personagens e objetos"),
    ("style/04_pepe_anchor.png", "sprite-âncora: densidade de pixels, contorno e sombreamento de referência"),
    ("style/04_pepe_anchor_native.png", "o mesmo sprite-âncora no tamanho real (24×32)"),
    ("style/05_pepe_legacy.png", "sprite antigo do Pepe (identidade)"),
]

# ---------------------------------------------------------------- shared blocks
STYLE = {
"ow": "Pixel art sprite in the style of Undertale and Deltarune overworld sprites, 3/4 top-down RPG view, crisp 1-pixel near-black outline (never dark purple: the Ruins floor is purple), flat cel shading with one shadow tone per color, very limited palette, no anti-aliasing, no gradients, no blur, chunky readable pixels, adult proportions, isolated on a solid flat #FF00FF magenta background, no watermark.",
"battle": "Undertale battle sprite, monochrome white and black pixel art (only #FFFFFF and #000000): white 1-pixel line art, light materials such as fur, faces and costume heads filled solid white, dark materials such as clothes filled solid black, a continuous unbroken white 1-pixel outline around the ENTIRE outer edge of the silhouette (also around black clothes, hair, ears and tail, so no black part of the figure ever touches the background), a few long continuous white lines for folds and seams instead of scattered dots, the whole silhouette fully opaque, bold readable silhouette, hand-drawn pixel art, no anti-aliasing, no gray, no gradients, front-facing, centered, isolated on a solid flat #FF00FF magenta background, no watermark.",
"ui": "Tiny pixel art game element in the style of Undertale, crisp hard pixels, no anti-aliasing, no gradients, flat colors, high contrast, isolated on a solid flat #FF00FF magenta background, no watermark.",
"tile": "Pixel art top-down RPG tileset in the style of Undertale's Ruins, 3/4 top-down view, crisp hard pixels, flat shading, no anti-aliasing, no gradients, no blur, limited palette, no watermark.",
"prop": "Single pixel art prop for a top-down RPG in the style of Undertale's Ruins, 3/4 top-down view, a continuous 1-pixel near-black #141018 outline closing the whole silhouette, flat cel shading, no anti-aliasing, no gradients, no blur, limited palette; it stands on a purple floor (#3B1F5C, #4A2A70, #5E3A87) against purple walls (#7A55A8, #9C7CC4), so its large areas use light or warm colours that contrast with purple and the Ruins purples appear only in small details, isolated on a solid flat #FF00FF magenta background, no text, no watermark.",
"card": "Pixel art story card in the style of Undertale's opening cutscene, limited palette, soft vignette, crisp pixels, no anti-aliasing, no gradients beyond the palette steps, no watermark.",
}
NEGATIVE = "blurry, anti-aliased, soft shading, smooth gradient, 3D render, realistic, photo, painterly, noisy texture, random dithering, jpeg artifacts, watermark, signature, brand logo, real person, real face, child proportions, baby face, nsfw, suggestive, cropped, cut off, inconsistent sizes between frames"

STYLE_RULES = {
"ow": ["Overworld é **colorido**, com paleta limitada (Ruins: roxo + verde neon).",
       "Vista 3/4 de cima, igual aos sprites de overworld do Undertale.",
       "Contorno de 1 px **quase preto `#141018`** fechando a silhueta inteira (nunca roxo escuro: o chão das Ruins é roxo e engole o contorno). O Pepe mantém o contorno próprio `#3D120E`. Uma sombra por cor; sem anti-aliasing."],
"battle": ["Batalha é **só branco `#FFFFFF` e preto `#000000`**, como a Toriel e o Flowey do Undertale: traço branco; materiais claros (pelo, rosto, cabeça de fantasia) preenchidos de branco, materiais escuros (roupa) de preto.",
           "**Contorno branco fechado em volta da silhueta inteira**, também em volta da roupa preta, do cabelo, das orelhas e do rabo: nenhum preto do personagem encosta no fundo (guia de estilo §3.1). A keyline preta de 2 px por fora é aplicada depois por `tools/art_prompts/battle_outline.py`.",
           "Fora da silhueta = **transparente**. O fundo das batalhas comuns tem uma grade verde que não pode aparecer através do personagem, por isso o interior precisa ser todo opaco (branco ou preto).",
           "Nada de cinza: \"meio-tom\" se faz com pixels brancos espaçados.",
           "Cor só em momentos especiais, e só onde esta ficha disser."],
"ui": ["Elemento de interface/projétil: formas sólidas, legíveis em 2×, sem anti-aliasing.",
       "Projéteis são brancos `#FFFFFF` (salvo indicação), com o núcleo sólido centralizado na célula (o hitbox será um retângulo menor no centro)."],
"tile": ["Tiles de **20×20** em grade exata, chão emendando sem costura.",
         "Paleta das Ruins (abaixo), vista 3/4 de cima."],
"prop": ["Mesma vista 3/4 do tileset das Ruins. **Legibilidade sobre roxo:** contorno de 1 px **quase preto `#141018`** fechando a silhueta inteira, e massas grandes em cores **claras ou quentes**, fora dos roxos do chão e da parede (`#3B1F5C` `#4A2A70` `#5E3A87` `#7A55A8` `#9C7CC4`), que ficam só para detalhes pequenos. (Regra de 2026-10-18: com a paleta só do tileset, booth, poltrona, interruptor e cabos sumiam no chão.)",
         "Texto em placas/banners: gere **sem texto** e escreva à mão depois em fonte pixel de 5 px (a IA não escreve legível nesse tamanho). Texto sempre em inglês, exatamente como indicado."],
"card": ["Card de intro: imagem única, sem transparência.",
         "Mesma linguagem visual dos outros cards (`lore1..9`): monocromático sépia/laranja."],
}

GLOBAL_RULES = """- **Escala:** arte em **1× nativo** (tela cheia = 320×240). O jogo desenha a **2×** com filtro *point* num canvas virtual de 640×480. Todos os tamanhos desta ficha são 1×.
- **Pixel:** sem anti-aliasing, sem gradiente suave, sem blur, sem sub-pixel.
- **Sprite sheet:** grade fixa, células do mesmo tamanho, sem espaçamento nem margem. **Uma linha por animação**, frames da esquerda para a direita **começando na coluna 0**. O loader não sabe começar em outra coluna, então **estados/variantes diferentes vão em linhas diferentes**. Célula vazia = transparente.
- **Pivô (`origin`):** o mesmo ponto (pé / base central) em todas as células.
- **Conteúdo:** nada explícito; proporções **adultas**; nenhum rosto real; nenhuma marca real (nada de logo Doritos / Mountain Dew; use "CITRUS BLAST", chips sem marca); todo texto dentro da arte em **inglês**.
- **Undertale:** pode servir de referência de tamanho/estilo, mas nada copiado vai para o repositório."""

PIPELINE = """1. Numa conversa nova do gerador, envie o pacote da seção 6 (o `.md` + até 4 imagens) e cole o prompt da seção 7.
2. Se o gerador errar a grade, gere **uma pose/frame por vez** (comece pelo parado de frente), aprove, e use o frame aprovado como referência para os outros; monte o sheet à mão. Alternativa: geradores nativos de pixel art (PixelLab, Retro Diffusion para Aseprite) já entregam no tamanho real.
3. No Aseprite / LibreSprite / Pixelorama: reduza para o tamanho nativo com **nearest neighbor** (ou redesenhe por cima), quantize para a paleta da seção 4, limpe o contorno para 1 px.
4. Monte na grade da seção 3, alinhe o pivô com uma guia, troque o magenta por transparência.
5. Exporte no formato da seção 3, escreva o JSON (seção \"JSON\", quando houver) e registre o arquivo em `assets/PLACEHOLDERS.md` (fonte/gerador, autor, licença)."""

RUINS_PALETTE = [("Base escura", "#3B1F5C"), ("Base média", "#5E3A87"), ("Acento neon", "#7CFF3A"),
                 ("Realce", "#F4E3FF"), ("Banners", "#C9A0DC"), ("Cortina 18+", "#111111"), ("Tela CRT", "#1E3FBF"),
                 ("Roxos intermediários (opcionais)", "#4A2A70 · #7A55A8 · #9C7CC4")]

# ---------------------------------------------------------------- helpers
def anim(name, row, frames=1, fps=None, loop=True):
    return (name, row, frames, fps, loop)

def walk4(extra=()):
    a = []
    for i, d in enumerate(["down", "up", "left", "right"]):
        n = 4 if d in ("down", "up") else 2
        a.append(anim("stand_" + d, i, 1))
        a.append(anim("walk_" + d, i, n, 6 if n == 4 else 4))
    return a + list(extra)

WALK4_LAYOUT = """| Linha | Animação | Col 0 | Col 1 | Col 2 | Col 3 |
| :---: | :--- | :--- | :--- | :--- | :--- |
| 0 | `walk_down` / `stand_down` | parado, de frente | passo pé esquerdo | parado (cópia da col 0) | passo pé direito |
| 1 | `walk_up` / `stand_up` | parado, de costas | passo esquerdo | parado (cópia) | passo direito |
| 2 | `walk_left` / `stand_left` | parado, perfil esquerdo | passo | *(vazio)* | *(vazio)* |
| 3 | `walk_right` / `stand_right` | parado, perfil direito | passo | *(vazio)* | *(vazio)* |
"""

def json_for(image, cell, origin, anims):
    lines = ["{", f'  "image": "{image}",', f'  "cell": [{cell[0]}, {cell[1]}],',
             f'  "origin": [{origin[0]}, {origin[1]}],', '  "anims": {']
    body = []
    for name, row, frames, fps, loop in anims:
        parts = [f'"row": {row}', f'"frames": {frames}']
        if fps is not None:
            parts.append(f'"fps": {fps}')
        if not loop:
            parts.append('"loop": false')
        body.append(f'    "{name}": {{ ' + ", ".join(parts) + " }")
    lines.append(",\n".join(body))
    lines += ["  }", "}"]
    return "\n".join(lines)

ASSETS = []
def add(**kw):
    ASSETS.append(kw)

# ---------------------------------------------------------------- A. overworld
add(id="A1", slug="ow_pepe_frog", file="sprites/ow/pepe_frog.png", title="Pepe (protagonista) — overworld",
    style="ow", core=True, code="**Sim** — `OverworldScene` carrega `pepe_frog.png` + `pepe_frog.json` e cai no BMP legado se faltar algum.",
    rooms="Todas (é o sprite que o jogador controla).",
    context="""Pepe the Fallen Gamer, paródia da Frisk. 26 anos, mora no porão da mãe, vive de raids, gacha e salgadinho. Sarcástico, preguiçoso, diz odiar furries (secretamente gosta — o jogo nunca diz). **Nunca fala** em voz alta.

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

**Decisão (2026-10-17):** o sprite segue o meme (traços do rosto + camiseta azul), substituindo o antigo \"moletom cinza\"; LORE §8.4/§10.1, ASSETS_NEEDED §4.1, BACKLOG e ROTEIRO já foram atualizados. O Pepe é um personagem com copyright do Matt Furie — vale reavaliar antes de um lançamento comercial.

**Decisão (2026-10-17, revisão):** as pontas dos dedos laranja saíram. Eram um aceno ao Undertale, mas na geração viraram mãos amarelas e o personagem deixou de ler como Pepe. A referência à Frisk fica no papel (protagonista calado), na pose dura de braços soltos e no layout de 4 direções do sheet, **não** em cor no corpo: a 20×30 px cada pixel colorido compete com os traços do meme.""",
    cell=(24, 32), grid=(4, 4), origin=(12, 31),
    fmt_notes=["Personagem ocupa ~22×31 dentro da célula (cabeça larga; corpo ~13 px).",
               "A linha 3 pode ser a linha 2 espelhada, mas **precisa existir no sheet** (o código não espelha).",
               "Peça ao gerador a mesma **densidade de pixels do sprite-âncora**, blocos uniformes e bordas 100% opacas (a 1ª geração saiu com ~25×37 px de arte, blocos irregulares e borda semitransparente)."],
    layout=WALK4_LAYOUT,
    anims=walk4(),
    palette=[("Contorno, linha entre os lábios", "#3D120E"), ("Pele / sombra", "#5FA33A / #3F7A2A"), ("Linha da olheira", "#2E5E22"),
             ("Olho branco / pupila", "#FFFFFF / #141414"), ("Lábio de cima / de baixo", "#C8553D / #A8432E"),
             ("Camiseta azul / sombra", "#2F62D6 / #1F449E"), ("Manchas de nacho (só na camiseta)", "#F0A03A"),
             ("Short (do legado) / sombra", "#67A4E0 / #4A80BC"), ("Chinelo", "#2B2B2B")],
    reference="""`assets/img/pepe_char.bmp` (66×90, grade 3×3 de células 22×30, fundo magenta), cópia limpa em `style/05_pepe_legacy.png`: linha 0 de frente, linha 1 perfis, linha 2 de costas. É a melhor referência de identidade antiga (camiseta azul, cabeça larga, lábio grosso). O **sprite-âncora** `style/04_pepe_anchor.png` é a versão nova de frente, já com os traços do meme — o sheet tem que bater com ele.""",
    prompt="""Character sprite sheet of a cartoon frog man drawn in the classic sad-frog internet comic look, a tired 26-year-old gamer; the front idle frame must match the anchor sprite exactly. Head: wide and lumpy, clearly wider than his shoulders, about 45% of body height, with two big round eye domes bulging above the top of the head. Eyes: large white eyes half covered by heavy droopy green eyelids (the lid line cuts each eye roughly in half), big black pupils resting on the lower lid, a bored half-lidded sideways stare, a faint darker-green line under each eye. Mouth: very wide thick two-part lips spanning almost the whole width of the face, reddish-brown upper lip #C8553D and darker lower lip #A8432E separated by a dark line, a slight frown, the lips protruding forward in profile. Skin: medium frog green #5FA33A with darker green #3F7A2A shading. Body: short and a little pudgy, plain blue crew-neck T-shirt #2F62D6 with one or two tiny orange nacho-cheese stains (small dots, not a logo), light blue shorts, black flip-flops #2B2B2B, plain green frog hands and legs in the same skin green as the face (no orange or yellow on the hands or feet), arms hanging loosely, slouched lazy posture. Strict grid of 4 rows, every frame the same size, feet on the same baseline: row 1 facing down (idle, left step, idle, right step), row 2 facing up showing the back of the head with the eye domes and the back of the T-shirt (idle, left step, idle, right step), row 3 facing left (idle, step), row 4 facing right (idle, step). Dark brown outline #3D120E. No text.""",
    cleanup=["Copie a cabeça da col 0 para todos os frames da linha e só mexa corpo/pés — lábio e olhos são o que mais \"pula\" entre frames.",
             "No frame de passo o corpo sobe 1 px.",
             "Confirme o pivô: pés tocando a linha y=31 em todas as células.",
             "**Lição da 1ª geração:** reduzir automaticamente o PNG grande para 24×32 borra os olhos, espalha pixels roxos e transforma o azul-marinho em preto. Use a imagem grande só como guia e **redesenhe a 1×** por cima, partindo do sprite-âncora."],
    accept=["Andar nas 4 direções no jogo sem o sprite \"tremer\" (pivô fixo).", "Lê como o Pepe a 2× (pálpebras + dois lábios + cabeça larga).",
            "Frame parado de frente igual ao sprite-âncora.", "Camiseta azul, não moletom.", "Mãos verdes, sem laranja/amarelo; chinelo preto."])

add(id="A2", slug="ow_pepe_wakeup", file="sprites/ow/pepe_wakeup.png", title="Pepe deitado / levantando (R1)",
    style="ow", core=False, code="Não. **Nome sugerido** — ainda não referenciado; o cutscene runner (CORE-011) pode usar.",
    rooms="R1 — The Fall Room.",
    context="""Abertura do jogo: Pepe acorda numa cama de embalagens de salgadinho sob um feixe de luz. Pensa: `* (Ugh... where's my wireless mouse? And why does it smell like body spray and wet dog?)`. Mesmo personagem da ficha A1 e do sprite-âncora: o sapo do meme (cabeça larga com domos dos olhos, pálpebras pesadas, dois lábios marrom-avermelhados), **camiseta azul** com manchas de nacho, short azul claro, chinelo.""",
    cell=(32, 32), grid=(3, 2), origin=(16, 31),
    fmt_notes=["Deitado ocupa ~30×12 na horizontal; por isso a célula é mais larga que a do A1.", "O último frame de `get_up` deve ser idêntico ao `stand_down` do A1."],
    layout="""| Linha | Animação | Frames |
| :---: | :--- | :--- |
| 0 | `lying` | 1: deitado de barriga para cima, olhos fechados, braços abertos |
| 1 | `get_up` (sem loop) | 3: sentado esfregando o olho → ajoelhado se apoiando → de pé de frente |
""",
    anims=[anim("lying", 0, 1), anim("get_up", 1, 3, 4, False)],
    palette=[("Contorno", "#3D120E"), ("Pele / sombra", "#5FA33A / #3F7A2A"), ("Linha da olheira", "#2E5E22"), ("Olho / pupila", "#FFFFFF / #141414"),
             ("Lábios", "#C8553D / #A8432E"), ("Camiseta / sombra", "#2F62D6 / #1F449E"), ("Nacho", "#F0A03A"), ("Short", "#67A4E0 / #4A80BC"), ("Chinelo", "#2B2B2B")],
    reference="Nenhuma. Use o sprite-âncora e o sheet final da ficha A1 como referência de personagem.",
    prompt="""Small sprite sheet of the same cartoon frog man as the anchor sprite, in the classic sad-frog comic look (wide lumpy head with two eye domes on top, heavy droopy eyelids, wide two-part reddish-brown lips, medium frog-green skin #5FA33A, plain blue T-shirt #2F62D6 with orange nacho stains, light blue shorts, black flip-flops, short pudgy adult body). Row 1: lying flat on his back on the ground, eyes closed, arms spread, seen from a 3/4 top-down view. Row 2, three frames: sitting up rubbing one eye, kneeling and pushing himself up, standing facing the viewer slouched. Same scale as a 20x30 pixel character, each frame in an equal 32x32 cell, feet aligned to the bottom. Dark brown outline #3D120E. No text.""",
    cleanup=["Reaproveite a cabeça/cores do A1 para manter o mesmo personagem.", "Alinhe o último frame pixel a pixel com o `stand_down` do A1."],
    accept=["A transição `get_up` → `stand_down` não dá salto visível."])

add(id="A3", slug="ow_furiell", file="sprites/ow/furiell.png", title="Furiell — overworld",
    style="ow", core=False, code="Não (ainda desenhada com primitivas / ausente). Formato pronto para o loader.",
    rooms="R2 (chegada), R3–R12 (guia o Pepe; casa dela em R11).",
    context="""Furiell, Guardiã das Ruins, paródia da Toriel. Ex-rainha; vive nas Ruins para \"converter com amor\" os gamers que caem. Campeã do *love-bombing*: mãe sufocante, gentil, docemente passivo-agressiva, chama todo mundo de \"my child\". Faz Pop-Tarts de glitter.

**Visual (LORE §10.2):** fursuit de cabra **branca e fofa**, óculos meia-lua, **avental com estampa de patinhas**, **zíper visível nas costas** (ela jura que não existe). Nunca é sexual com o Pepe; o \"nojento\" dela é o que ela coleciona.""",
    cell=(32, 44), grid=(4, 5), origin=(16, 43),
    fmt_notes=["Personagem ~26×40 dentro da célula.", "O zíper prateado tem que aparecer na linha de costas (linha 1)."],
    layout=WALK4_LAYOUT + "| 4 | `talk_down` | boca fechada | boca aberta | *(vazio)* | *(vazio)* |\n",
    anims=walk4([anim("talk_down", 4, 2, 4)]),
    palette=[("Contorno", "#2A1640"), ("Pelo branco / sombra lilás", "#F4F0EA / #CFC3DD"), ("Chifres", "#E6D8B8"),
             ("Avental / estampa de patinhas", "#C9A0DC / #5E3A87"), ("Vestido", "#5E3A87"), ("Óculos (aro dourado)", "#E6B422"),
             ("Zíper", "#9A9AAA"), ("Olhos", "#3B1F5C")],
    reference="Nenhuma no repositório. Silhueta da Toriel (alta, ombros largos, vestido longo), mas claramente uma **fursuit**: cabeçorra fofa, dobra de tecido no pescoço.",
    prompt="""Character sprite sheet of a tall motherly adult woman wearing a fluffy white goat fursuit (oversized soft costume head with small cream horns and floppy ears, a visible fabric fold at the neck), small gold half-moon reading glasses, a long purple dress under a lilac apron with a dark purple paw-print pattern, kind smile, gentle posture with paws clasped. Seen from behind, a silver costume zipper runs clearly down the middle of her back. Strict grid of 5 rows, all frames the same size, feet on the same baseline: row 1 facing down (idle, left step, idle, right step), row 2 facing up with the zipper visible (idle, left step, idle, right step), row 3 facing left (idle, step), row 4 facing right (idle, step), row 5 facing down talking (mouth closed, mouth open). White fur #F4F0EA with lilac shadows, apron #C9A0DC, dark purple outline #2A1640. Adult proportions, about 26x40 pixels per frame. No text.""",
    cleanup=["Mantenha os chifres e orelhas idênticos entre frames.", "Zíper: 1 px de largura, 3–4 px de dentes visíveis."],
    accept=["Lê como fursuit (não como cabra real).", "Zíper visível de costas.", "Proporções adultas."])

add(id="A4", slug="ow_barky", file="sprites/ow/barky.png", title="Barky — overworld",
    style="ow", core=True, code="Não. Formato pronto para o loader (a entrada está em `data/cutscenes/barky_entrance.json`).",
    rooms="R9 (arena; entrada caindo do teto) e depois como NPC se poupado.",
    context="""Barky the Alpha Wolf, mini-boss das Ruins. Ex-`xX_BarkSc0p3_Xx`, **23 anos**, o 7º convertido (2021). Edgy, dramático, uiva para lâmpadas fluorescentes, lidera a \"Shadow Fang Pack\" (membros: ele). Inseguro: acha que todo mundo percebe que ele é \"novo\". Tem flashbacks de gamer.

**Visual (LORE §10.3):** orelhas de lobo, jaqueta de couro falso com tachas, correntes de plástico, luvas sem dedo, **rabo preso com alfinete**, fursuit barata com **zíper visível**. **Silhueta:** as orelhas.

**Entrada (ROTEIRO cena 3):** um uivo, Barky cai do teto e aterrissa agachado.""",
    cell=(28, 36), grid=(4, 5), origin=(14, 35),
    fmt_notes=["Personagem ~22×32; a folga em cima é para as orelhas."],
    layout=WALK4_LAYOUT + "| 4 | `drop` (sem loop) | caindo, braços para cima, orelhas achatadas | aterrissagem agachado, punho no chão | levantando em pose dramática | *(vazio)* |\n",
    anims=walk4([anim("drop", 4, 3, 6, False)]),
    palette=[("Contorno", "#141418"), ("Pelo / sombra", "#9A9AAA / #6E6E7E"), ("Interior das orelhas", "#C98AA8"),
             ("Jaqueta / realce", "#232329 / #3C3C46"), ("Tachas, correntes plásticas", "#C8C8D2"), ("Luvas", "#232329"),
             ("Mecha emo na franja", "#7CFF3A"), ("Olhos (delineador)", "#D0263A"), ("Alfinete / zíper", "#9A9AAA")],
    reference="Nenhuma. Na batalha ele é desenhado com primitivas em `BattleSystem.cpp` (corpo, cabeça, orelhas, rabo) — só serve de referência de partes.",
    prompt="""Character sprite sheet of an edgy 23-year-old man in a cheap gray wolf fursuit: huge pointy wolf ears, emo side-swept fringe with one neon green streak, red eyeliner eyes, black studded faux-leather jacket, plastic silver chains, fingerless gloves, a fluffy wolf tail attached with a big safety pin, a visible costume zipper on his back. Dramatic try-hard posture, arms crossed when idle. Strict grid of 5 rows, all frames the same size, feet on the same baseline: row 1 facing down (idle, left step, idle, right step), row 2 facing up showing the zipper and safety-pinned tail (idle, left step, idle, right step), row 3 facing left (idle, step), row 4 facing right (idle, step), row 5 entrance (falling from above with arms up, superhero landing crouch with one fist on the floor, standing up into a dramatic pose). Adult proportions, about 22x32 pixels per frame, the ears must read clearly as a black silhouette. No text.""",
    cleanup=["Orelhas idênticas em todos os frames de caminhada.", "No frame de queda o pivô continua em [14,35] (o código move a posição)."],
    accept=["Silhueta em preto reconhecível pelas orelhas.", "Alfinete e zíper visíveis de costas."])

add(id="A5", slug="ow_floofy", file="sprites/ow/floofy.png", title="Floofy — overworld",
    style="ow", core=False, code="Não. Formato pronto para o loader.",
    rooms="R2 (tutorial falso dos \"free hug pellets\"), R13 (saída).",
    context="""Floofy, a flor sem vergonha, paródia do Flowey (e do Asriel). Flor dourada usando a **cabeça de fursuit do Príncipe Azzy**. Sem alma, **sem vergonha**. Fofa e falsa no começo; depois debochada e sem limites. Fala: `Howdy! I'm FLOOFY! Floofy the Flower!`

**Visual (LORE §10.10):** flor amarela de pétalas grandes; o \"rosto\" é uma cabecinha de fursuit de cabra com olhos de plástico e **um zíper descendo pelo meio da cara**. No modo sinistro o zíper abre e mostra **só escuridão e dois olhos brancos** (nunca nada além disso).""",
    cell=(24, 24), grid=(3, 4), origin=(12, 23),
    fmt_notes=["Personagem ~20×20.", "Pivô = onde o caule entra no chão."],
    layout="""| Linha | Animação | Frames |
| :---: | :--- | :--- |
| 0 | `idle` | 2: balançando 1 px para os lados |
| 1 | `talk` | 2: **sem boca** (quem fala é o que está dentro da fantasia): a cabeça balança 1 px e o puxador do zíper chacoalha |
| 2 | `sinister` | 2: zíper aberto, escuridão + dois olhos brancos (frame 2: olhos 1 px menores) |
| 3 | `sink` (sem loop) | 3: afundando no chão (inteiro → metade → só pétalas) |
""",
    anims=[anim("idle", 0, 2, 2), anim("talk", 1, 2, 4), anim("sinister", 2, 2, 3), anim("sink", 3, 3, 6, False)],
    palette=[("Contorno", "#2A1A08"), ("Pétalas / sombra", "#FFD23F / #E6A800"), ("Caule e folhas", "#3FA34D / #2C7A38"),
             ("Cabeça de fursuit", "#F4F0EA / #CFC3DD"), ("Olhos de plástico", "#111111 + brilho #FFFFFF"), ("Zíper", "#9A9AAA"),
             ("Interior aberto", "#000000 + olhos #FFFFFF")],
    reference="Nenhuma. Silhueta do Flowey com a cabeça de mascote no lugar do rosto.",
    prompt="""Small sprite sheet of a golden yellow flower with big rounded petals and a green stem with two leaves, whose face is a tiny cute white baby-goat mascot costume head with shiny black plastic eyes, a silver zipper running down the middle of the face from the top of the head to the chin, with the pull tab at the chin, and no mouth. Strict grid of 4 rows, equal 24x24 cells, stem base at the bottom center: row 1 idle gently swaying (2 frames), row 2 talking (2 frames, no mouth: the voice comes from whatever is inside the costume, so talking is shown only by the head bobbing 1 pixel and the zipper pull tab swinging), row 3 sinister with the face zipper pulled open revealing only pitch-black darkness and two small glowing white dot eyes (2 frames), row 4 sinking into the ground (full, half, only petals visible). About 20x20 pixels of character per frame. No text.""",
    cleanup=["No modo sinistro: o interior é **preto puro** com dois pontos brancos — nada mais."],
    accept=["Lê como flor + cabeça de mascote mesmo em 20×20.", "Regra de conteúdo do modo sinistro respeitada."])

# ---------------------------------------------------------------- B. battle
add(id="B1", slug="battle_barky", file="sprites/battle/barky.png", title="Barky — batalha (mini-boss)",
    style="battle", core=True, code="**Sim** — `BattleSystem` carrega `barky.png` + `barky.json` e usa `idle`, `hurt`, `blush` (após Cringe Check) e `spared` (desenhado translúcido). Sem os arquivos, usa as primitivas.",
    rooms="R9 — Barky's Arena (fundo: grade verde + um tubo fluorescente no topo).",
    context="""Barky the Alpha Wolf (ver também A4). Intro: `Halt, hairless intruder! *[awkward howling]* You have stepped into the domain of the Shadow Fang Pack! Prepare to face my fursona!`

Check: `* BARKY - ATK 4 DEF 2` / `* Alpha Wolf of the Ruins. The zipper says otherwise.`
ACTs relevantes para as poses: **Cringe Check** (encara o zíper → `H-hey! It's an artisanal design feature, b-baka!` → pose `blush`), Offer Dew, Debate Lore, Laser, **Accept Hug** (`...this is the best day of my life. Don't tell anyone.`).

**Visual:** orelhas grandes pontudas, franja emo cobrindo um olho, jaqueta com tachas, correntes de plástico, luvas sem dedo, rabo com alfinete, **zíper visível**. A LORE pede o Barky \"em partes\" (orelhas, jaqueta, correntes, rabo) para animar como os inimigos do Undertale no futuro.""",
    cell=(60, 80), grid=(2, 4), origin=(30, 80),
    fmt_notes=["`cell` e `origin` já são os documentados em `assets/sprites/README.md` — não mude.",
               "Entregue também o arquivo de trabalho (`.aseprite`/`.psd`) com cada parte numa camada (orelhas, cabeça, jaqueta, correntes, rabo)."],
    layout="""| Linha | Animação | Frames | Detalhe |
| :---: | :--- | :--- | :--- |
| 0 | `idle` | 2 | pose edgy de braços cruzados; frame 2: orelha esquerda dobra 2 px e o rabo vai para o outro lado |
| 1 | `hurt` | 1 | olhos fechados apertados, orelhas achatadas, recuando |
| 2 | `blush` | 1 | corando, olhando de lado, patas tapando o zíper — **única cor permitida:** bochechas `#FF77C8` |
| 3 | `spared` | 1 | relaxado, sorriso tímido |
""",
    anims=[anim("idle", 0, 2, 2), anim("hurt", 1, 1), anim("blush", 2, 1), anim("spared", 3, 1)],
    palette=[("Traço", "#FFFFFF"), ("Preenchimento interno (opaco)", "#000000"), ("Blush (só linha 2)", "#FF77C8")],
    reference="Primitivas em `src/combat/BattleSystem.cpp` (blocos de corpo, cabeça, orelhas e rabo) — só referência de proporção/partes.",
    prompt="""Battle sprite sheet of an edgy man in a cheap wolf fursuit: big pointy ears, an emo fringe covering one eye, a studded leather jacket, plastic chains, fingerless gloves, a fluffy tail held by a safety pin, a costume zipper visible at the side of the neck. Half-body to full-body, front view, each frame in an equal 60x80 cell. Strict grid of 4 rows: row 1 idle with arms crossed (2 frames, ear twitch and tail sway), row 2 hurt (squeezed-shut eyes, flattened ears, recoiling), row 3 embarrassed, blushing, looking away and covering the zipper with his paws, row 4 relaxed shy smile. Only exception to monochrome: small pink #FF77C8 blush marks in row 3. No text.""",
    cleanup=["Garanta preto **opaco** em todo o interior (teste sobre uma grade verde).", "Pés/base tocando y=80 em todas as células."],
    accept=["No jogo: idle anima, hurt aparece ao acertar, blush após Cringe Check, spared translúcido.", "Nenhum pixel cinza."])

add(id="B2", slug="battle_furiell", file="sprites/battle/furiell.png", title="Furiell — batalha (boss)",
    style="battle", core=False, code="Não. Formato pronto para o loader.",
    rooms="R12 — porão (fundo: preto liso).",
    context="""Furiell (ver A3). Antes da luta: `Every gamer who leaves comes back worse. Angrier. Lonelier. Filmed. ...Prove to me you can survive up there.`
Check: `* FURIELL - ATK 6 DEF 1` / `* Knows what's best for you. Has forty blankets to prove it.`
O Spare repetido leva às falas tristes (`...Ha ha. I can't even keep a single gamer.`) → pose `sad`. Accept Hug encerra a luta.

**Visual:** como a Toriel de batalha (busto grande, mangas largas), mas com cabeçorra de fursuit de cabra, óculos meia-lua, avental de patinhas e o zíper aparecendo na lateral do pescoço.""",
    cell=(80, 110), grid=(2, 3), origin=(40, 110),
    fmt_notes=["Enquadramento da cintura/joelhos para cima, centralizado."],
    layout="""| Linha | Animação | Frames | Detalhe |
| :---: | :--- | :--- | :--- |
| 0 | `idle` | 2 | patas juntas na frente do avental; frame 2: respiração (ombros 1 px) e brilho nos óculos |
| 1 | `hurt` | 1 | olhos fechados, recuando, óculos tortos |
| 2 | `sad` | 1 | cabeça baixa, uma pata no peito |
""",
    anims=[anim("idle", 0, 2, 2), anim("hurt", 1, 1), anim("sad", 2, 1)],
    palette=[("Traço", "#FFFFFF"), ("Preenchimento interno (opaco)", "#000000")],
    reference="Nenhuma.",
    prompt="""Battle sprite sheet of a tall motherly woman in a fluffy white goat fursuit: oversized soft costume head with small horns and floppy ears, half-moon reading glasses, a long robe with wide sleeves, an apron with a paw-print pattern, a costume zipper barely visible at the neck. Front view, waist-up to the knees, each frame in an equal 80x110 cell. Strict grid of 3 rows: row 1 idle with paws clasped in front (2 frames, subtle breathing, glasses glint), row 2 hurt (eyes shut, recoiling, glasses askew), row 3 sad (head lowered, one paw on her chest). No text.""",
    cleanup=["Interior todo opaco (branco ou preto), sem buracos transparentes.", "Estampa de patinhas simplificada (3–4 patinhas legíveis)."],
    accept=["Lê como fursuit, com proporções adultas.", "Três poses claramente distintas."])

add(id="B3", slug="battle_floofy", file="sprites/battle/floofy.png", title="Floofy — batalha (tutorial)",
    style="battle", core=False, code="Não. Formato pronto para o loader.",
    rooms="R2 — tutorial dos \"free hug pellets\".",
    context="""Floofy (ver A5). Tutorial falso: `Down here, we share these little white... "free hug pellets"!` Os \"pellets\" são coraçõezinhos que causam dano e +20 FUR; ele cerca o Pepe com um anel (`HUG.`). Depois revela: `You idiot. In this world, it's SHAME or be SHAMED.` → modo sinistro. Furiell chega e espanta ele com spray de glitter.""",
    cell=(40, 40), grid=(2, 3), origin=(20, 40),
    fmt_notes=["**Decisão de cor:** pétalas podem ser amarelas `#FFD23F` (o Flowey do tutorial aparece em cor). Se usar, use também nos retratos (C2)."],
    layout="""| Linha | Animação | Frames |
| :---: | :--- | :--- |
| 0 | `idle` | 2 |
| 1 | `talking` | 2 |
| 2 | `sinister` | 2 — zíper aberto, só escuridão + dois olhos brancos |
""",
    anims=[anim("idle", 0, 2, 2), anim("talking", 1, 2, 4), anim("sinister", 2, 2, 3)],
    palette=[("Traço", "#FFFFFF"), ("Preenchimento interno (opaco)", "#000000"), ("Pétalas (opcional)", "#FFD23F")],
    reference="Nenhuma.",
    prompt="""Battle sprite sheet of a flower with big rounded petals and a short stem, its face a cute baby-goat mascot costume head with plastic eyes, a silver zipper running down the middle of the face from the top of the head to the chin, with the pull tab at the chin, and no mouth. Front view, each frame in an equal 40x40 cell. Strict grid of 3 rows: row 1 idle swaying (2 frames), row 2 talking (2 frames, no mouth: the voice comes from whatever is inside the costume, so talking is shown only by the head bobbing 1 pixel and the zipper pull tab swinging), row 3 sinister with the zipper open showing only black darkness and two white dot eyes (2 frames). No text.""",
    cleanup=["Modo sinistro: nada dentro do zíper além de preto e dois pontos brancos."],
    accept=["Silhueta legível em 40×40."])

add(id="B4", slug="battle_fursuit_dummy", file="sprites/battle/fursuit_dummy.png", title="Fursuit Dummy — batalha (tutorial)",
    style="battle", core=False, code="Não. Formato pronto para o loader.",
    rooms="R5 — tutorial de batalha (ensina Talk e Accept Hug).",
    context="""Paródia do Training Dummy. HP 15 · AT 0 · DF −5.
Check: `* FURSUIT DUMMY - ATK 0 DEF -5` / `* A mannequin wearing a fursuit head. Smells like the previous owner.`
Accept Hug: `* You hug the dummy. It's damp. FUR +10.` — Furiell: `You can always accept a hug, my child.`""",
    cell=(40, 60), grid=(2, 2), origin=(20, 60),
    fmt_notes=[],
    layout="""| Linha | Animação | Frames |
| :---: | :--- | :--- |
| 0 | `idle` | 2: parado / tecido da cabeça caído 1 px |
| 1 | `hurt` | 1: inclinado, enchimento saindo de uma costura |
""",
    anims=[anim("idle", 0, 2, 1), anim("hurt", 1, 1)],
    palette=[("Traço", "#FFFFFF"), ("Preenchimento interno (opaco)", "#000000")],
    reference="Nenhuma (o Dummy do Undertale serve de referência de tamanho).",
    prompt="""Battle sprite of a stuffed cloth training dummy on a wooden post wearing a cheap, slightly crooked animal fursuit head with button eyes and droopy ears, visible stitches, looking a little damp. Front view, each frame in an equal 40x60 cell. Row 1: idle (2 frames, the head fabric sagging 1 pixel). Row 2: hurt, tilted with a bit of stuffing poking out of a seam. No text.""",
    cleanup=["Costuras com pixels brancos tracejados."], accept=["Lê como boneco de treino + cabeça de fursuit."])

COMMON_FMT = ["Grade padrão dos inimigos comuns: linha 0 `idle` (2 frames), linha 1 `hurt` (1 frame).", "Pivô = centro da base."]
COMMON_LAYOUT = """| Linha | Animação | Frames |
| :---: | :--- | :--- |
| 0 | `idle` | 2 |
| 1 | `hurt` | 1 |
"""
def common(id, slug, name, parody, cell, rooms, ctx, prompt, ref_note="Nenhuma."):
    add(id=id, slug="battle_" + slug, file=f"sprites/battle/{slug}.png", title=f"{name} — batalha (inimigo comum, paródia de {parody})",
        style="battle", core=False, code="Não. Formato pronto para o loader.", rooms=rooms, context=ctx,
        cell=cell, grid=(2, 2), origin=(cell[0] // 2, cell[1]), fmt_notes=COMMON_FMT, layout=COMMON_LAYOUT,
        anims=[anim("idle", 0, 2, 2), anim("hurt", 1, 1)],
        palette=[("Traço", "#FFFFFF"), ("Preenchimento interno (opaco)", "#000000")], reference=ref_note,
        prompt=prompt + f" Each frame in an equal {cell[0]}x{cell[1]} cell. No text.",
        cleanup=["Interior todo opaco (branco ou preto), sem buracos transparentes.", "Frame 2 do idle muda só 1–2 px (balanço), para não \"pular\"."],
        accept=["A piada do personagem lê sem texto."])

common("B5", "froggsona", "Froggsona", "Froggit", (40, 40), "R8 (encontro aleatório; pool com Shysona e Modsmal).",
       """HP 30 · AT 4 · DF 2. Check: `* FROGGSONA - ATK 4 DEF 2` / `* A frog fursona. Every sentence ends in ":3".`
Balões: `ribbit :3` · `*ribbits suggestively*`. Pensamento do Pepe: `* (It looks like you. A little. Don't think about it.)`
ACTs: Compliment (`* You say "nice tail". It doesn't have a tail. It's flattered anyway.`), Threaten, Accept Hug.
**Visual:** sapinho numa fursuit caseira de sapo, olhos de fantasia enormes, boca de gato `:3`, coleira com sininho.""",
       """Battle sprite of a small round frog-character in a homemade frog fursuit, huge glossy costume eyes, a cat-like ":3" mouth, a little collar with a bell, squatting pose. Front view, about 40x40 pixels. Row 1: idle (2 frames, small hop squash). Row 2: hurt (eyes as X marks, flattened).""")
common("B6", "shysona", "Shysona", "Whimsun", (40, 40), "R8 (encontro aleatório).",
       """HP 10 · AT 5 · DF 0. Check: `* SHYSONA - ATK 5 DEF 0` / `* A brand-new fursona. Apologizes for existing. And for its commissions.`
Balões: `Sorry...` · `Sorry for the art...` · `You weren't supposed to see that...`. ACTs: Console, Terrorize, Accept Hug.
**Visual:** criaturinha tímida com asas de mariposa, escondendo metade do rosto choroso atrás de um caderno de desenho, antenas caídas.""",
       """Battle sprite of a shy tiny moth-winged furry creature hovering, hiding half its teary face behind a small sketchbook, antennae drooping, apologetic posture. Front view, about 40x40 pixels. Row 1: idle floating (2 frames, wings flutter). Row 2: hurt (curled up, tears).""")
common("B7", "modsmal", "Modsmal", "Moldsmal", (40, 30), "R8 (encontro aleatório).",
       """HP 50 · AT 1 · DF 0. Check: `* MODSMAL - ATK 1 DEF 0` / `* A gelatinous moderator of the NSFW channel. Wobbles. Enforces. Sometimes.`
Balões: `*squorch* (Read the pins.)` · `*squelch* (Tag your art.)`. ACTs: Imitate, Flirt (`* You say "nice ban hammer". Rule 7 bends a little.`).
**Visual:** gelatina em forma de pudim com carinha sonolenta, um distintivo de escudo escrito \"MOD\" em cima e um martelinho de ban de gelatina.""",
       """Battle sprite of a wobbly jelly-mold blob with a tiny sleepy face and a small shield-shaped badge stuck on top, a tiny gelatin ban hammer leaning against it. Front view, about 40x30 pixels. Row 1: idle wobble (2 frames, squash and stretch). Row 2: hurt (splattered flat).""",
       "Nenhuma. O texto \"MOD\" do distintivo deve ser desenhado à mão (3×5 px por letra) depois.")
common("B8", "lurkeye", "Lurkeye", "Loox", (50, 50), "R10 (encontro aleatório; pool com Reply Guy e Grasstoid).",
       """HP 50 · AT 6 · DF 4. Check: `* LURKEYE - ATK 6 DEF 4` / `* Has watched the 18+ channel since 2016. Has never typed. Not once.`
Balões: `...` · `(typing...)` · `(stopped typing)`. ACTs: Don't Pick On, Pick On.
**Visual:** criatura redonda que é basicamente um olho gigante com olheiras, perninhas curtas, segurando um celular aceso na frente do rosto, balão \"...\" ao lado.""",
       """Battle sprite of a round creature that is mostly one giant staring eye with heavy dark under-eye bags, short stubby legs, holding a glowing smartphone up to its face, a tiny "..." typing bubble beside it. Front view, about 50x50 pixels. Row 1: idle (2 frames, slow blink). Row 2: hurt (eye squeezed shut, phone dropped).""")
common("B9", "reply_guy", "Reply Guy", "Migosp", (40, 56), "R10 (encontro aleatório).",
       """HP 40 · AT 6 · DF 4. Check: `* REPLY GUY - ATK 6 DEF 4` / `* Has a correction about your fursona's species.`
Balões: `Well, actually, that's a coyote.` · `Source?` · sozinho: `...yeah, it's a wolf, man.`
**Visual:** cara magricelo meio inseto, antenas longas, óculos redondos grandes, um dedo levantado (\"well, actually\"), a outra mão empurrando os óculos.""",
       """Battle sprite of a lanky insect-like guy with long antennae and big round glasses, one finger raised in a "well, actually" gesture, the other hand pushing up his glasses, smug posture. Front view, about 40x56 pixels. Row 1: idle (2 frames, finger wagging). Row 2: hurt (glasses cracked, recoiling).""")
common("B10", "grasstoid", "Grasstoid", "Vegetoid", (50, 60), "R10 (encontro aleatório).",
       """HP 72 · AT 6 · DF 6. Check: `* GRASSTOID - ATK 6 DEF 6` / `* A living patch of grass. Nobody down here has touched it. Ever.`
Balões: `Touch me.` · `Please. Nobody ever does.` · `Outside is NICE.`. ACTs: **Touch** (FUR −10, poupa — a piada do \"touch grass\"), Dinner.
**Visual:** tufo de grama viva num quadrado de grama, carinha solitária esperançosa no meio, dois bracinhos de grama pedindo abraço.""",
       """Battle sprite of a living clump of tall grass rooted in a square patch of turf, with a hopeful lonely face in the middle and two little grass arms reaching out for a hug. Front view, about 50x60 pixels. Row 1: idle (2 frames, blades swaying). Row 2: hurt (blades flattened, sad face).""")

# ---------------------------------------------------------------- C. portraits
def portrait(id, who, files, ctx, prompt):
    add(id=id, slug=f"portraits_{who}", file=f"portraits/{who}_*.png", title=f"Retratos de diálogo — {who.capitalize()}",
        style="battle", core=False, code="Não (sem loader de retratos ainda). Entrega em arquivos soltos.",
        rooms="Caixas de diálogo em que o personagem fala.", context=ctx,
        files=files, single=(50, 50),
        fmt_notes=["**Um PNG por expressão**, 50×50 cada, nomes exatamente como na tabela.",
                   "Gere todas as expressões **numa imagem só** (mesma seed) para consistência e depois recorte.",
                   "Enquadramento: cabeça e ombros, cabeça ocupando ~70% da altura, **mesma posição** em todos.",
                   "Preto opaco dentro da silhueta, transparente fora (o retrato fica dentro da caixa preta de diálogo)."],
        palette=[("Traço", "#FFFFFF"), ("Preenchimento interno (opaco)", "#000000")],
        reference="Nenhuma. Referência de estilo: os retratos da Toriel no Undertale (traço branco).",
        prompt=prompt + " No text.",
        cleanup=["Alinhe olhos e queixo na mesma altura em todos os arquivos (troca de expressão não pode \"pular\")."],
        accept=["Expressões distinguíveis em 50×50.", "Mesmo personagem em todos."])

portrait("C1", "furiell",
         [("furiell_neutral.png", "sorriso suave, olhos semicerrados"), ("furiell_happy.png", "olhos fechados em ^^, sorriso aberto"),
          ("furiell_sad.png", "sobrancelhas para cima, boca curva para baixo"), ("furiell_angry.png", "olhos estreitos, óculos com brilho, boca reta"),
          ("furiell_flustered.png", "\"fechou a aba\": olhos arregalados, gota de suor, olhando de lado")],
         "Furiell (ver A3/B2): mãe sufocante e gentil, numa fursuit de cabra branca com óculos meia-lua e avental. A expressão *flustered* é o momento em que ela fecha uma aba do navegador muito rápido quando o Pepe entra (`* She closes a tab very fast.`).",
         "Dialogue portrait sheet, 5 portraits in one row, identical framing in each (head and shoulders, 50x50 pixels each): a motherly woman in a fluffy white goat fursuit head with small horns, floppy ears and half-moon reading glasses, apron straps visible. Expressions left to right: gentle neutral smile, happy with closed ^^ eyes, sad with raised brows, stern angry with the glasses glinting, flustered wide-eyed with a sweat drop looking away as if caught closing a browser tab.")
portrait("C2", "floofy",
         [("floofy_cute.png", "olhos grandes e brilhantes de mascote, sem boca"), ("floofy_smug.png", "olhos semicerrados + um sorriso torto de 1 linha: a única boca do Floofy, só porque o deboche não lê sem ela"),
          ("floofy_sinister.png", "zíper aberto: só escuridão + dois olhos brancos")],
         "Floofy (ver A5/B3): flor com cabeça de fursuit de cabritinho e zíper no meio da cara. Fofo e falso; depois sem limites. Se as pétalas forem amarelas no B3, use o mesmo aqui.",
         "Dialogue portrait sheet, 3 portraits in one row, identical framing (50x50 pixels each): a flower whose face is a baby-goat mascot costume head with plastic eyes and a silver zipper running down the middle of the face from the top of the head to the chin, with the pull tab at the chin; the costume has no mouth. Expressions: cute (big shiny innocent eyes, no mouth), smug (half-lidded eyes and one thin crooked grin line, the only mouth this character ever shows), sinister with the zipper pulled open showing only darkness and two white dot eyes.")
portrait("C3", "barky",
         [("barky_edgy.png", "olhar de lado sombrio, franja no olho"), ("barky_embarrassed.png", "corando, orelhas para trás (blush #FF77C8 opcional)"),
          ("barky_happy.png", "sorriso aberto, orelhas em pé")],
         "Barky (ver A4/B1): edgy, dramático, inseguro. *Embarrassed* = depois do Cringe Check (`H-hey! It's an artisanal design feature, b-baka!`); *happy* = `...this is the best day of my life. Don't tell anyone.`",
         "Dialogue portrait sheet, 3 portraits in one row, identical framing (50x50 pixels each): a man in a cheap wolf fursuit head with huge pointy ears, an emo fringe over one eye, a studded jacket collar. Expressions: edgy brooding side glance, embarrassed blushing with ears pinned back, genuinely happy wide grin with ears up.")

# ---------------------------------------------------------------- D. bullets
def bullet(id, slug, user, ctx, cell, grid, layout, anims, prompt, palette=None, notes=()):
    add(id=id, slug="bullet_" + slug, file=f"bullets/{slug}.png", title=f"Projétil `{slug}` — {user}",
        style="ui", core=False, code="Não (hoje os projéteis são primitivas). JSON abaixo é a sugestão para quando o loader de bullets existir.",
        rooms=user, context=ctx, cell=cell, grid=grid, origin=(cell[0] // 2, cell[1] // 2),
        fmt_notes=["Pivô no centro (projéteis giram/movem pelo centro).", "Deixe ≥1 px transparente em volta do desenho."] + list(notes),
        layout=layout, anims=anims, palette=palette or [("Projétil", "#FFFFFF")], reference="Nenhuma (primitivas no código).",
        prompt=prompt, cleanup=["Núcleo sólido e centralizado; nada de pontas finas de 1 px que pareçam acertar sem tocar."],
        accept=["Legível em movimento a 2×.", "Silhueta clara sobre fundo preto."])

def strip_layout(name, desc):
    return f"| Linha | Animação | Frames |\n| :---: | :--- | :--- |\n| 0 | `{name}` | {desc} |\n"

bullet("D1", "heart_pellet", "Floofy (R2) e o padrão `hug_ring` da Furiell",
       "Os \"free hug pellets\" do Floofy: coraçõezinhos brancos que causam dano e +20 FUR cada. Ele os dispara e depois cerca o Pepe com um anel deles. A Furiell reaproveita no `hug_ring`.",
       (8, 8), (2, 1), strip_layout("spin", "2: coração de frente / de lado (giro)"), [anim("spin", 0, 2, 6)],
       "Tiny solid white heart-shaped bullet, 8x8 pixels, two frames of a spin (front view, then edge-on), equal 8x8 cells in one row.")
bullet("D2", "uwu_letters", "Barky — padrão `uwu_rain`",
       "Chuva de letras \"U\" e \"W\" caindo (hoje é texto desenhado em código). Precisa parecer letra fofa, não fonte de sistema.",
       (12, 14), (1, 2), "| Linha | Animação | Frames |\n| :---: | :--- | :--- |\n| 0 | `u` | 1: letra U |\n| 1 | `w` | 1: letra W |\n",
       [anim("u", 0, 1), anim("w", 1, 1)],
       "Two chunky bold white pixel letters, a U and a W, rounded cute bubbly style, each in an equal 12x14 cell, stacked vertically (U on top, W below).",
       notes=["Letras em **linhas separadas** porque o código escolhe qual letra pela linha."])
bullet("D3", "can", "Barky — padrão `dew_bounce`",
       "Latinhas de refrigerante quicando nas bordas da caixa de batalha (\"Citrus Blast\" — sem marca real).",
       (14, 14), (4, 1), strip_layout("spin", "4: rotações de 0°, 45°, 90°, 135°"), [anim("spin", 0, 4, 8)],
       "Small white soda can bullet with a pull tab, black inner details, four rotation frames (0, 45, 90, 135 degrees) in equal 14x14 cells in one row, no brand.")
bullet("D4", "sound_ring", "Barky — `howl_wave`; Lurkeye — `seen_rings`",
       "Ondas sonoras do uivo desafinado do Barky (aparece depois do Cringe Check) e os anéis de \"visto\" do Lurkeye.",
       (32, 32), (1, 3), "| Linha | Animação | Frames |\n| :---: | :--- | :--- |\n| 0 | `small` | 1: anel Ø12 |\n| 1 | `medium` | 1: anel Ø20 |\n| 2 | `large` | 1: anel Ø30 |\n",
       [anim("small", 0, 1), anim("medium", 1, 1), anim("large", 2, 1)],
       "Three white sound-wave ring bullets, each a 2-pixel-thick circle outline with a small gap, sizes 12, 20 and 30 pixels across, each centered in an equal 32x32 cell, stacked vertically.",
       notes=["Tamanhos em linhas separadas."])
bullet("D5", "fluff_ball", "Furiell — padrão `fluff_balls`",
       "Bolinhas de pelo fofas que a Furiell arremessa (+6 FUR).",
       (12, 12), (2, 1), strip_layout("pulse", "2: fofura pulsando"), [anim("pulse", 0, 2, 4)],
       "Fluffy round white fur-ball bullet with a jagged fuzzy edge, two frames pulsing slightly, equal 12x12 cells in one row.")
bullet("D6", "glitter_hand", "Furiell — padrão `glitter_hands`",
       "Patas de fursuit com brilhos que varrem a caixa (versão Furiell das mãos de fogo da Toriel).",
       (30, 30), (2, 1), strip_layout("grab", "2: pata aberta / fechando"), [anim("grab", 0, 2, 4)],
       "White fluffy costume paw glove with small sparkle glints around it, two frames: open paw and closing paw, equal 30x30 cells in one row.")
bullet("D7", "colon_three", "Froggsona — padrão `colon_three_hop`",
       "O emoticon \":3\" pulando em arcos.",
       (12, 12), (2, 1), strip_layout("hop", "2: normal / achatado no pulo"), [anim("hop", 0, 2, 6)],
       "A white \":3\" emoticon drawn as a chunky pixel face, two frames: normal and squashed, equal 12x12 cells in one row.")
bullet("D8", "fly", "Froggsona — padrão `fly_swarm`",
       "Enxame de mosquinhas (comida de sapo).", (8, 8), (2, 1), strip_layout("buzz", "2: asas para cima / para baixo"), [anim("buzz", 0, 2, 10)],
       "Tiny white fly bullet, two frames: wings up and wings down, equal 8x8 cells in one row.")
bullet("D9", "moth", "Shysona — padrão `sorry_moths`",
       "Mariposinhas tímidas que voam pedindo desculpa.", (12, 10), (2, 1), strip_layout("flap", "2: asas abertas / fechadas"), [anim("flap", 0, 2, 8)],
       "Tiny white moth bullet, two frames: wings open and wings closed, equal 12x10 cells in one row.")
bullet("D10", "gel_hammer", "Modsmal — padrão `ban_gel`",
       "Martelinhos de ban feitos de gelatina.", (12, 16), (2, 1), strip_layout("wobble", "2: inclinado para um lado / outro"), [anim("wobble", 0, 2, 4)],
       "Small wobbly white gelatin ban-hammer bullet, two frames tilting left and right, equal 12x16 cells in one row.")
bullet("D11", "text_bug", "Reply Guy — padrão `well_actually`",
       "Insetinhos feitos de linhas de texto rabiscado (as \"correções\" do Reply Guy).", (16, 8), (2, 1), strip_layout("crawl", "2: perninhas alternando"), [anim("crawl", 0, 2, 8)],
       "Small white bug bullet whose body is made of tiny scribbled text lines, six little legs alternating between two frames, equal 16x8 cells in one row, no readable letters.")
bullet("D12", "grass_blade", "Grasstoid — padrão `grass_blades`",
       "Lâminas de grama. As **verdes curam** (convenção do Undertale) — por isso há duas cores.",
       (8, 16), (2, 2), "| Linha | Animação | Frames |\n| :---: | :--- | :--- |\n| 0 | `white` | 2: balanço |\n| 1 | `green` | 2: balanço (verde, cura) |\n",
       [anim("white", 0, 2, 4), anim("green", 1, 2, 4)],
       "Thin pointed grass-blade bullet, two sway frames per row, equal 8x16 cells: top row pure white, bottom row bright green #00FF00.",
       palette=[("Lâmina que fere", "#FFFFFF"), ("Lâmina que cura", "#00FF00")], notes=["Cores em linhas separadas."])
bullet("D13", "tear", "Napstablurk — padrão `seen_tears` (opcional, P2)",
       "Lágrimas que caem (1 de dano). O Napstablurk é opcional; esta é a menor prioridade da lista.",
       (6, 8), (1, 1), strip_layout("idle", "1"), [anim("idle", 0, 1)],
       "Tiny white teardrop bullet, 6x8 pixels.")

# ---------------------------------------------------------------- E. UI
add(id="E1", slug="ui_buttons", file="ui/buttons.png", title="Botões de batalha FIGHT / ACT / ITEM / MERCY",
    style="ui", core=True, code="Não (hoje: retângulo + texto em `BattleSystem::drawButtons`).", rooms="Toda batalha.",
    context="""Os quatro botões do menu de batalha (o código desenha cada um em 110×42 = 2× de 55×21). Normal = laranja `#FF7F27`, selecionado = amarelo `#FFFF00`, MERCY travado = cinza `#808080` (cores de `include/core/Constants.hpp`). No botão selecionado o código desenha a **alma vermelha no lugar do ícone** (BUI-006, igual ao Undertale).

Ícones paródia: FIGHT = mira de FPS, ACT = balão de fala, ITEM = saquinho de salgadinho, MERCY = bandeira branca com uma patinha. Letras grossas \"feitas à mão\".""",
    cell=(55, 21), grid=(1, 9), origin=(0, 0),
    fmt_notes=["Um botão por linha (o loader seleciona pela linha).", "Borda de 1 px, ícone em x 2–15, texto à direita, interior preto opaco.",
               "Nas linhas **selected** deixe a área do ícone **vazia (preta)** — a alma é desenhada ali pelo código."],
    layout="""| Linha | Animação | Conteúdo |
| :---: | :--- | :--- |
| 0 | `fight` | FIGHT laranja com ícone |
| 1 | `fight_selected` | FIGHT amarelo, sem ícone |
| 2 | `act` | ACT laranja |
| 3 | `act_selected` | ACT amarelo, sem ícone |
| 4 | `item` | ITEM laranja |
| 5 | `item_selected` | ITEM amarelo, sem ícone |
| 6 | `mercy` | MERCY laranja |
| 7 | `mercy_selected` | MERCY amarelo, sem ícone |
| 8 | `mercy_locked` | MERCY cinza com ícone (P1) |
""",
    anims=[anim(n, i, 1) for i, n in enumerate(["fight", "fight_selected", "act", "act_selected", "item", "item_selected", "mercy", "mercy_selected", "mercy_locked"])],
    palette=[("Normal", "#FF7F27"), ("Selecionado", "#FFFF00"), ("Travado", "#808080"), ("Interior", "#000000")],
    reference="Nenhuma imagem; o desenho atual é feito em código.",
    prompt="""Undertale-style battle menu buttons, each button 55x21 pixels: a 1-pixel rectangular border, a small icon on the left and chunky hand-drawn bold capital letters on the right, black fill. Nine buttons stacked vertically in equal cells: "FIGHT" orange #FF7F27 with an FPS crosshair icon; "FIGHT" yellow #FFFF00 with the icon area empty; "ACT" orange with a speech-bubble icon; "ACT" yellow with the icon area empty; "ITEM" orange with a small chip-bag icon; "ITEM" yellow with the icon area empty; "MERCY" orange with a white flag icon that has a paw print; "MERCY" yellow with the icon area empty; "MERCY" gray #808080 with the flag icon.""",
    cleanup=["A IA vai errar as letras: use a fonte do jogo (`assets/fonts/8bitoperator.ttf`) ou desenhe à mão com 7 px de altura.", "Ícones com no máximo 12×12."],
    accept=["Texto exatamente FIGHT / ACT / ITEM / MERCY.", "Área do ícone vazia nas linhas *selected*."])

add(id="E2", slug="ui_souls", file="ui/souls.png", title="Almas (SOUL) do jogador",
    style="ui", core=True, code="Não (hoje: `drawSoul` com primitivas).", rooms="Toda batalha + menus.",
    context="""A alma que o jogador controla na caixa de batalha. Cores dos modos clássicos (de `Constants.hpp`) + duas do nUndertale: **pelúcia rosa com orelhinhas** (modo FURRY) e **sapo** (a alma gamer do Pepe). O hitbox no código é 8×8 e não muda.""",
    cell=(12, 12), grid=(1, 6), origin=(6, 6),
    fmt_notes=["Coração 8×8 centralizado na célula 12×12; orelhas/olhos podem ocupar a folga.", "Uma alma por linha.", "Linhas futuras (P2): rachada, estilhaços, sequência FURRY OVER."],
    layout="""| Linha | Animação | Alma | Cor |
| :---: | :--- | :--- | :--- |
| 0 | `red` | normal | `#FF0000` |
| 1 | `blue` | gravidade | `#0026FF` |
| 2 | `green` | escudo | `#00C000` |
| 3 | `yellow` | tiro | `#FFFF00` |
| 4 | `plush` | pelúcia rosa com orelhinhas | `#FFB3E6` + costura `#FF77C8` |
| 5 | `frog` | sapo, olhinhos no topo | `#6FC418` + olhos `#FFFFFF`/`#000000` |
""",
    anims=[anim(n, i, 1) for i, n in enumerate(["red", "blue", "green", "yellow", "plush", "frog"])],
    palette=[("Vermelha", "#FF0000"), ("Azul", "#0026FF"), ("Verde", "#00C000"), ("Amarela", "#FFFF00"), ("Pelúcia / costura", "#FFB3E6 / #FF77C8"), ("Sapo", "#6FC418")],
    reference="Nenhuma imagem; desenho atual em código.",
    prompt="""Six tiny pixel heart souls stacked vertically, each an 8x8 pixel heart centered in an equal 12x12 cell: solid red #FF0000, solid blue #0026FF, solid green #00C000, solid yellow #FFFF00, a pink plush heart #FFB3E6 with two tiny fluffy ears and a stitched seam #FF77C8, and a green frog heart #6FC418 with two small bulging eyes on top.""",
    cleanup=["Os 4 corações clássicos devem ter **exatamente** o mesmo formato."], accept=["Legível a 2× (16×16 na tela)."])

add(id="E3", slug="ui_target", file="ui/target.png", title="Mira do FIGHT (target)",
    style="ui", core=False, code="Não (hoje: primitivas).", rooms="Toda batalha, ao escolher FIGHT.",
    context="A mira elíptica onde a barra corre no ataque (equivalente ao alvo do Undertale). Paródia gamer: um retículo de FPS no centro. Cabe na caixa de batalha (287 px nativos de largura).",
    single=(280, 65), fmt_notes=["Imagem única, sem sheet.", "Fundo: transparente fora da elipse; interior pode ser preto opaco."],
    palette=[("Linhas", "#FFFFFF"), ("Fundo interno", "#000000")], reference="Nenhuma.",
    prompt="""Undertale-style FIGHT attack target, exactly 280x65 pixels: a wide white elliptical eye-shaped target with horizontal guide lines, concentric bands narrowing toward a bright center, and a small FPS crosshair reticle in the exact center, on black.""",
    cleanup=["Simetria horizontal perfeita; o centro exato em x=140."], accept=["Centro claramente marcado."])

add(id="E4", slug="ui_attack_bar", file="ui/attack_bar.png", title="Barra de ataque do FIGHT",
    style="ui", core=False, code="Não (hoje: primitiva).", rooms="Toda batalha, ao escolher FIGHT.",
    context="A barra vertical que atravessa a mira (E3); o jogador aperta Z para parar. Depois do golpe ela pisca.",
    cell=(7, 65), grid=(2, 1), origin=(3, 32), fmt_notes=[],
    layout=strip_layout("flash", "2: branco sólido / contorno branco com miolo preto"), anims=[anim("idle", 0, 1), anim("flash", 0, 2, 10)],
    palette=[("Barra", "#FFFFFF"), ("Miolo", "#000000")], reference="Nenhuma.",
    prompt="A 7x65 pixel vertical bar, two frames side by side: one solid white, one with a white 1-pixel outline and a black core.",
    cleanup=[], accept=["Contraste máximo sobre a mira."])

add(id="E5", slug="ui_slash", file="ui/slash.png", title="Golpe (hitmarker de FPS)",
    style="ui", core=False, code="Não (hoje: primitiva).", rooms="Toda batalha, ao acertar.",
    context="Efeito de acerto no inimigo: em vez do corte do Undertale, um **hitmarker de FPS** (o \"X\" de quatro traços). Mesma duração do slash do Undertale (6 frames).",
    cell=(30, 30), grid=(6, 1), origin=(15, 15), fmt_notes=["Opcional: 2ª linha `crit` em vermelho `#FF0000`."],
    layout=strip_layout("hit", "6: surgindo do centro (1–3), cheio (4), sumindo (5–6)"), anims=[anim("hit", 0, 6, 15, False)],
    palette=[("Hitmarker", "#FFFFFF"), ("Crítico (opcional)", "#FF0000")], reference="Nenhuma.",
    prompt="Six-frame animation strip of a classic FPS hitmarker: four short diagonal white strokes forming an X around an empty center, growing outward from the center, full, then fading out, equal 30x30 cells in one row.",
    cleanup=["O centro fica vazio em todos os frames."], accept=["Reconhecível como hitmarker de FPS."])

add(id="E6", slug="ui_exclaim", file="ui/exclaim.png", title="\"!\" do encontro",
    style="ui", core=False, code="Não (hoje: primitiva).", rooms="Overworld, ao começar um encontro.",
    context="O balão com \"!\" que aparece sobre a cabeça do Pepe quando um encontro começa (ENC-001/002).",
    single=(10, 12), fmt_notes=["Imagem única."], palette=[("Balão", "#FFFFFF"), ("Contorno e \"!\"", "#000000")], reference="Nenhuma.",
    prompt="Tiny 10x12 pixel exclamation speech bubble: a white rounded bubble with a 1-pixel black outline, a small tail at the bottom, a bold black \"!\" inside.",
    cleanup=[], accept=["Legível a 2× sobre o overworld roxo."])

add(id="E7", slug="ui_furry_over", file="ui/furry_over.png", title="Logo \"FURRY OVER\"",
    style="ui", core=False, code="Não (hoje: texto).", rooms="Tela de FURRY OVER (HP zerado / FUR cheio).",
    context="O equivalente ao GAME OVER do Undertale. Letras enormes brancas, mas **felpudas**, com tufos de pelo; opcional uma orelhinha saindo do \"O\".",
    single=(210, 90), fmt_notes=["Imagem única, fundo transparente."], palette=[("Letras", "#FFFFFF")], reference="Nenhuma.",
    prompt="Game-over title logo reading \"FURRY OVER\" in two stacked lines, huge chunky white pixel letters in the style of Undertale's GAME OVER screen, but every letter edge is fuzzy and fluffy like fur with small fur tufts, on black, 210x90 pixels. The text must be exactly \"FURRY OVER\".",
    cleanup=["Corrija as letras à mão se a IA errar."], accept=["Texto exato.", "Mesmo peso visual do GAME OVER do Undertale."])

add(id="E8", slug="ui_logo", file="ui/logo.png", title="Logo do título (\"UNDERTALE\" sem o n)",
    style="ui", core=False, code="Não (hoje: texto).", rooms="Tela de título.",
    context="Logo `nUNDERTALE`: o \"n\" minúsculo vermelho treme levemente, por isso fica num arquivo separado (E9). Este arquivo tem só \"UNDERTALE\" em branco com o espaço do \"n\" à esquerda.",
    single=(200, 30), fmt_notes=["Deixe ~16 px livres à esquerda para o \"n\" (E9)."], palette=[("Letras", "#FFFFFF")], reference="Nenhuma.",
    prompt="Title logo text \"UNDERTALE\" in wide white serif pixel capitals similar to the Undertale logo, on black, about 200x30 pixels, with empty space on the left for one extra letter.",
    cleanup=["Letras à mão se preciso."], accept=["Alinha com o E9 na mesma linha de base."])

add(id="E9", slug="ui_logo_n", file="ui/logo_n.png", title="Logo do título — o \"n\" vermelho",
    style="ui", core=False, code="Não. **Nome sugerido.**", rooms="Tela de título.",
    context="O \"n\" minúsculo vermelho do `nUNDERTALE`, em camada separada para o código fazê-lo tremer.",
    single=(14, 20), fmt_notes=["Mesma linha de base do E8."], palette=[("Letra", "#FF0000")], reference="Nenhuma.",
    prompt="A single lowercase letter \"n\" in red #FF0000, serif pixel style matching the Undertale logo, about 14x20 pixels.",
    cleanup=[], accept=["Mesmo estilo de serifa do E8."])

add(id="E10", slug="props_save_point", file="props/save_point.png", title="Ponto de save (estrela na garrafa)",
    style="ui", core=True, code="Não (hoje: primitiva).", rooms="R1, R3, R13 (e todos os saves depois).",
    context="""O ponto de save: uma estrela dourada de 4 pontas girando **em cima de uma garrafa de refrigerante** genérica (\"CITRUS BLAST\", sem marca real). Texto do save na R1: `* The scent of lukewarm energy drinks and stale Doritos fills you with DETERMINATION.`""",
    cell=(20, 28), grid=(8, 1), origin=(10, 27), fmt_notes=["Garrafa parada; só a estrela gira (8 fases)."],
    layout=strip_layout("spin", "8: fases de rotação da estrela"), anims=[anim("spin", 0, 8, 8)],
    palette=[("Estrela / brilho / contorno", "#FFD23F / #FFF6B0 / #8A5A00"), ("Garrafa / sombra", "#3FA34D / #2C7A38"), ("Rótulo", "#7CFF3A"), ("Tampa", "#E6E6E6")],
    reference="Nenhuma imagem; hoje é uma primitiva.",
    prompt="Eight-frame animation strip in equal 20x28 cells: a small green soda bottle with a lime label and a white cap standing still at the bottom, and floating above it a golden yellow four-pointed sparkle star (Undertale save-point style) rotating through 8 phases of a spin. No brand logos, no text.",
    cleanup=["Copie a garrafa idêntica para os 8 frames."], accept=["Loop da rotação sem salto."])

UI_PALETTE = {"E11": [("Branco", "#FFFFFF"), ("Preto", "#000000"), ("HP", "#FFFF00"), ("HP perdido", "#FF0000"), ("Overlay FUR", "#B04CFF")]}
for id_, slug, file, title, size, ctx, prompt in [
    ("E11", "ui_hud", "ui/hud.png", "Peças do HUD (opcional)", (60, 10),
     "Barra de HP (amarelo `#FFFF00` sobre vermelho `#FF0000`), overlay roxo do FUR e rótulo \"FUR\". **O código já desenha bem com primitivas** — só faça se quiser um visual desenhado à mão.",
     "Pixel art HUD pieces on black: a short horizontal HP bar segment in yellow #FFFF00, the same segment in red #FF0000, a purple #B04CFF overlay segment, and the small label \"FUR\" in chunky white pixel letters."),
    ("E12", "ui_dialog_box", "ui/dialog_box.png", "Caixa de diálogo (opcional)", (289, 76),
     "Caixa de diálogo do Undertale: borda branca grossa, interior preto. **Pode continuar no código.** Se fizer, faça em 9-slice (cantos fixos).",
     "Undertale dialogue box, 289x76 pixels, thick 3-pixel white border, solid black interior, square corners."),
    ("E13", "ui_portrait_frame", "ui/portrait_frame.png", "Moldura do retrato (opcional)", (52, 52),
     "Moldura em volta do retrato de 50×50 dentro da caixa de diálogo. **Pode continuar no código.**",
     "A 52x52 pixel square frame with a 1-pixel white border and a transparent center."),
]:
    add(id=id_, slug=slug, file=file, title=title, style="ui", core=False, code="Não. Opcional.", rooms="Batalha / diálogo.",
        context=ctx, single=size, fmt_notes=["Imagem única."], palette=UI_PALETTE.get(id_, [("Branco", "#FFFFFF"), ("Preto", "#000000")]),
        reference="Nenhuma; desenhado em código.", prompt=prompt, cleanup=[], accept=["Encaixa nas posições atuais do código."])

# ---------------------------------------------------------------- F. tiles + props
add(id="F1", slug="tiles_ruins", file="tiles/ruins.png", title="Tileset das Ruins",
    style="tile", core=True, code="Não. Depende do loader de mapas do Tiled (OVW-003).", rooms="R1–R13.",
    context="""**Conceito (LORE §12):** os salões originais da **FurNation Con '14**, afundados há 12 anos — \"uma convenção furry abandonada de 2014 dentro das ruínas roxas do Undertale\". Pedra roxa rachada misturada com carpete de convenção, banners desbotados, tendas de lona, a Art Show com a seção 18+ atrás de uma cortina preta. Embalagens de salgadinho no chão (crocantes).

Substitui o fundo atual do overworld (`assets/img/test.bmp`, que é o papel de parede \"Bliss\" do Windows XP — foto com copyright, não pode ir para build público).""",
    cell=(20, 20), grid=(8, 8), origin=None,
    fmt_notes=["Tileset para o **Tiled** (não usa o JSON de sprite).", "Tiles de chão emendam sem costura em todas as direções.", "~55 dos 64 slots usados; slots livres ficam transparentes."],
    layout="""| Linha | Col 0–7 |
| :---: | :--- |
| 0 | pedra A · pedra B · pedra C · pedra rachada · pedra com glitter · embalagem amassada no chão · migalhas de chips · buraco/vazio preto |
| 1 | carpete de convenção (padrão anos 90, roxo com zigue-zague verde neon): canto sup-esq · topo · canto sup-dir · esquerda · centro · direita · carpete gasto · mancha |
| 2 | carpete: canto inf-esq · base · canto inf-dir · fita adesiva no chão · tapete de boas-vindas · pegada de pata · *(livre)* · *(livre)* |
| 3 | parede topo (borda): esq · meio · dir · parede frente: esq · meio · dir · frente rachada · frente com duto de ventilação |
| 4 | canto interno esq · canto interno dir · canto externo esq · canto externo dir · frente com gancho de banner · frente com tomada + cabo · sombra na base da parede · parede escura |
| 5 | escada: topo esq · topo meio · topo dir · meio esq · meio meio · meio dir · base esq · base dir |
| 6 | porta fechada (topo, base) · porta aberta (topo, base) · arco: sup-esq · sup-dir · inf-esq · inf-dir |
| 7 | pilar: topo · meio · base · placa neon de fundo · feixe de luz semitransparente (topo, base) · borda de desnível · *(livre)* |
""",
    anims=None, palette=RUINS_PALETTE, reference="Nenhuma utilizável (o `test.bmp` atual é a foto Bliss).",
    prompt="""Top-down RPG tileset, 20x20 pixel tiles on a strict grid of 8 columns by 8 rows: cracked purple stone floor variants, a 1990s convention carpet with a purple and neon green zigzag pattern with 9-slice edges, purple brick walls with top caps, front faces, inner and outer corners, a wall with a ventilation duct, a wall with a power outlet and a cable, stairs, doors closed and open, an archway, pillars, a pale light beam, scattered snack wrappers and chip crumbs on the floor. Palette strictly #3B1F5C, #5E3A87, #7CFF3A, #F4E3FF, #C9A0DC. An abandoned furry convention from 2014 inside ancient purple ruins. Seamless floor tiles, no text.""",
    cleanup=["A IA não respeita grade de tiles: use o resultado como referência de textura e redesenhe/recorte cada tile em 20×20.", "Teste a emenda copiando cada tile de chão 3×3."],
    accept=["Chão sem costura visível.", "Só cores da paleta das Ruins."])

def prop(id, slug, title, rooms, ctx, size, prompt, grid=None, layout=None, anims=None, text=None, notes=(), core=False, extra=None):
    kw = dict(id=id, slug="props_" + slug, file=f"props/{slug}.png", title=title, style="prop", core=core,
              code="Não. Depende do loader de mapas/objetos (OVW-003).", rooms=rooms, palette=RUINS_PALETTE + ([extra] if extra else []),
              reference="Nenhuma.", cleanup=["Mesma luz (de cima/esquerda) e contorno do tileset F1."],
              accept=["Combina com o tileset F1 lado a lado."])
    ctx_full = ctx + "\n\n**Conceito da área:** convenção furry de 2014 abandonada dentro das ruínas roxas do Undertale."
    if text:
        ctx_full += f"\n\n**Texto na arte (inglês, exato):** `{text}` — gere o prop sem texto e escreva à mão em fonte pixel de 5 px."
        kw["accept"] = kw["accept"] + ["Texto exatamente como especificado."]
    kw["context"] = ctx_full
    if grid:
        kw.update(cell=size, grid=grid, origin=(size[0] // 2, size[1] - 1), layout=layout, anims=anims,
                  fmt_notes=["Estados/variantes em linhas separadas; frames de animação na mesma linha."] + list(notes))
    else:
        kw.update(single=size, fmt_notes=["Imagem única."] + list(notes))
    kw["prompt"] = prompt + (" Required colours for this prop: the %s use %s." % (extra[0].lower(), extra[1].replace(" / ", ", ")) if extra else "")
    add(**kw)

prop("F2", "ruins_banner_welcome", "Banner principal da convenção", "R3 — Ruins Entrance (escadaria dupla).",
     "Banner de pano creme com faixa lilás, rasgado numa ponta, pendurado por duas cordas cinza, com emblema de patinha rosa e uma faixa lisa para o texto.", (60, 40),
     "Faded cream cloth convention banner (#F4F0EA, folds #D8CDE8) with a lilac #B89AD8 border stripe, hanging from two gray ropes, torn at one corner, a small pink #F07AAE paw-print emblem, a flat blank cream band for text, 60x40 pixels.",
     text="FurNation Con '14 — Welcome!")
prop("F3", "ruins_banner_deodorant", "Faixa \"Deodorant is NOT optional\"", "R3 — embaixo do banner principal.",
     "Faixa de papel improvisada, presa com fita crepe nas pontas, logo abaixo do banner F2.", (60, 16),
     "Narrow handmade paper sign taped to a wall with masking tape at both ends, blank for text, 60x16 pixels.", text="Deodorant is NOT optional")
prop("F4", "ruins_booth", "Tenda de vendedor abandonada", "R8 (Art Show) e corredores.",
     "Mesa com toalha creme sob um toldo listrado rosa e creme em postes de madeira, pelúcias, prints e doces coloridos.", (60, 40),
     "Convention vendor booth: a table with a cream tablecloth (#F4F0EA) under a pink #F07AAE and cream striped canvas canopy on two wooden poles, a few colourful plushies, art prints and pastries on the table, 60x40 pixels.")
prop("F5", "ruins_curtain_18", "Cortina 18+ da Art Show", "R8 — Art Show.",
     "Cortina preta `#111111` pesada, **fechada**, numa barra, com uma plaquinha no meio. Nada visível atrás (regra de conteúdo). Examinar: `* You decide to respect the curtain.`", (40, 40),
     "Heavy closed black velvet curtain #111111 hanging from a rod, a small square sign pinned to the middle, blank for text, nothing visible behind it, 40x40 pixels.", text="18+")
prop("F6", "ruins_crt", "Monitor CRT com tela azul", "R8 — o Toy Laser Pointer fica atrás dele.",
     "CRT bege no chão com uma tela azul da morte `#1E3FBF`. Examinar: `* A blue screen of death. The error message is in Comic Sans.`", (20, 20),
     "Chunky beige CRT monitor on the floor showing a blue screen of death #1E3FBF with tiny unreadable white lines, two frames with a flickering scanline, equal 20x20 cells in one row.",
     grid=(2, 1), layout=strip_layout("flicker", "2: tela azul / scanline deslocada"), anims=[anim("flicker", 0, 2, 4)])
prop("F7", "ruins_notice_board", "Mural de avisos", "R8.",
     "Quadro de cortiça com moldura, papéis presos com tachinha e um aviso oficial grande. Examinar: `* NOTICE FROM RUINS ADMINISTRATION: Please do not shed in the ventilation shafts...`", (40, 30),
     "Cork notice board with a wooden frame, several pinned papers and one large official-looking notice, 40x30 pixels.", text="RUINS ADMINISTRATION (opcional, no aviso grande)")
prop("F8", "ruins_rgb_switch", "Interruptor RGB", "R4 — puzzle dos interruptores.",
     "Interruptor gamer de parede com fita LED RGB. Os corretos estão **marcados com glitter**. Bilhete: `Not this one. — Ruins Administration (not Furiell)`.", (20, 20),
     "Wall-mounted gaming light switch: a light gray-white plate (#E8E0F0) with a toggle lever and a short RGB LED strip, three states stacked vertically in equal 20x20 cells: off, lever down, LEDs dark; on, lever up, LEDs glowing red #E83A3A, green #3FE05A, blue #3A7AF0; off but sprinkled with bright yellow #FFE14A and pink #FF77C8 glitter sparkles, obvious from afar.",
     grid=(1, 3), layout="| Linha | Animação | Estado |\n| :---: | :--- | :--- |\n| 0 | `off` | desligado |\n| 1 | `on` | ligado, RGB |\n| 2 | `marked` | desligado + glitter (o certo) |\n",
     anims=[anim("off", 0, 1), anim("on", 1, 1), anim("marked", 2, 1)])
prop("F9", "ruins_dorito_bowl", "Tigela de chips", "R10.",
     "Tigela num pedestal com chips triangulares laranja `#F0A03A`, **sem marca**. Placa: `* \"Take one Dorito.\"` — pegar 4 derruba a tigela.", (20, 20),
     "Small white ceramic pedestal bowl (#F4F0EA) of orange triangular tortilla chips (no brand), three states stacked vertically in equal 20x20 cells: full; empty; knocked over with chips scattered on the floor.",
     grid=(1, 3), layout="| Linha | Animação | Estado |\n| :---: | :--- | :--- |\n| 0 | `full` | cheia |\n| 1 | `empty` | vazia |\n| 2 | `knocked` | derrubada, chips espalhados |\n",
     anims=[anim("full", 0, 1), anim("empty", 1, 1), anim("knocked", 2, 1)], text="Take one (opcional, numa plaquinha)",
     extra=("Chips", "#F0A03A / #C0701E"))
prop("F10", "ruins_wrapper_pile", "Pilha de embalagens de salgadinho", "R1 (cama onde o Pepe acorda) e chão crocante.",
     "Embalagens amassadas e brilhantes em cores apagadas. Variante pequena = chão crocante; variante larga = a \"cama\" da R1 sob o feixe de luz.", (40, 20),
     "Pile of crumpled shiny snack wrappers and empty chip bags in mixed muted colors, two variants stacked vertically in equal 40x20 cells: a small crunchy pile; a wide flat bed of wrappers. No brands.",
     grid=(1, 2), layout="| Linha | Animação | Variante |\n| :---: | :--- | :--- |\n| 0 | `pile` | pilha pequena |\n| 1 | `bed` | cama larga (R1) |\n",
     anims=[anim("pile", 0, 1), anim("bed", 1, 1)])
prop("F11", "ruins_cables", "Cabos que dão choque (os \"espinhos\")", "R6 — Cable Maze (Furiell guia o Pepe pela mão).",
     "Feixe vertical de cabos de rede (azul, branco e verde) que atravessa o tile de cima a baixo, para os tiles empilhados formarem uma barreira contínua. Ligados = faíscas amarelas e verdes; desligados = cabos cinza apagados (passáveis).", (20, 20),
     "Hazard barrier tile: a vertical bundle of three thick ethernet cables (blue #2F62D6, white #E8E8F0, green #3FA34D) running straight from the top edge to the bottom edge so stacked tiles form one continuous column, near-black outline on the left and right sides only; top row: live, with yellow #FFE14A and neon #7CFF3A sparks in two animation frames; bottom row: off, the same cables in dull gray. Equal 20x20 cells.",
     grid=(2, 2), layout="| Linha | Animação | Frames |\n| :---: | :--- | :--- |\n| 0 | `on` | 2: faíscas alternando |\n| 1 | `off` | 1: apagado |\n",
     anims=[anim("on", 0, 2, 8), anim("off", 1, 1)], notes=["Pivô ignorado (é tile de chão de 20×20)."])
prop("F12", "ruins_fireplace", "Lareira", "R11 — sala da casa da Furiell.",
     "Lareira aconchegante de pedra roxa, fogo laranja/amarelo, uma pelúcia secando na cornija. A casa \"cheira a lavanda e fursuit\".", (40, 40),
     "Cozy purple stone fireplace with a crackling orange and yellow fire, a small plush toy drying on the mantel, three animation frames of the fire in equal 40x40 cells in one row.",
     grid=(3, 1), layout=strip_layout("burn", "3: fogo"), anims=[anim("burn", 0, 3, 6)],
     extra=("Flames", "#FFD24A / #F0A03A / #E0602A"))
prop("F13", "ruins_armchair", "Poltrona felpuda", "R11.",
     "Poltrona grande de estofado vermelho quente, de frente, com uma manta de pelo falso creme dobrada sobre um dos braços.", (30, 30),
     "Big comfy armchair in warm red upholstery (#B0383E), front 3/4 view, short dark wooden legs, a folded cream faux-fur throw draped over one armrest, 30x30 pixels.")
prop("F14", "ruins_bed", "Cama do quarto do Pepe", "R11 — quarto do Pepe.",
     "Cama de solteiro, cobertor de pelo falso lilás, travesseiro com estampa de patinha. Examinar: `* You nap under the faux-fur blanket. HP fully restored. FUR +5.`", (40, 30),
     "Single bed seen from a 3/4 top-down view with a lilac faux-fur blanket and a pillow with a paw print, 40x30 pixels.")
prop("F15", "ruins_closet", "Armário (com a Fursuit Head)", "R11 — quarto do Pepe.",
     "Guarda-roupa de madeira. Aberto, mostra uma cabeça de fursuit na prateleira: `* A spare fursuit head. It's your size. Why is it your size?`", (30, 40),
     "Wooden wardrobe closet, two states stacked vertically in equal 30x40 cells: closed; open with a spare animal fursuit head sitting on the shelf inside.",
     grid=(1, 2), layout="| Linha | Animação | Estado |\n| :---: | :--- | :--- |\n| 0 | `closed` | fechado |\n| 1 | `open` | aberto, cabeça de fursuit na prateleira |\n",
     anims=[anim("closed", 0, 1), anim("open", 1, 1)])
prop("F16", "ruins_mirror", "Espelho do corredor", "R11 — corredor.",
     "Espelho alto com moldura roxa ornamentada. Examinar: `* It's you!` → com FUR ≥ 20: `* It's... still you. With ears.` O reflexo do Pepe é desenhado pela cena, não no prop.", (20, 40),
     "Tall standing hallway mirror with an ornate purple frame, two states stacked vertically in equal 20x40 cells: normal cool reflective glass; glass with a soft pink glow #FFB3E6.",
     grid=(1, 2), layout="| Linha | Animação | Estado |\n| :---: | :--- | :--- |\n| 0 | `normal` | vidro frio |\n| 1 | `fur` | brilho rosado `#FFB3E6` |\n",
     anims=[anim("normal", 0, 1), anim("fur", 1, 1)])
prop("F17", "ruins_bookshelf", "Estante", "R11.",
     "Livros coloridos desbotados, um volume grosso saindo um pouco (\"Fanfics of Questionable Quality, Vol. 7\" — só a lombada, sem texto legível).", (40, 40),
     "Wooden bookshelf full of faded colorful books, one thick volume sticking out slightly, 40x40 pixels.")
prop("F18", "ruins_lamp", "Luminária de globo (\"a lua do Barky\")", "R9 — Barky's Arena (também no fundo da batalha dele).",
     "Luminária de chão alta com um globo redondo amarelo-pálido que parece uma lua cheia. Examinar: `* Barky's natural enemy. Or his moon. He hasn't decided.`", (20, 40),
     "Tall standing floor lamp with a big round glowing globe shade like a full moon (pale yellow #FFF2B8, crescent shading #F2D06A, white highlight) on a thin gray pole with a small round base, two frames in equal 20x40 cells in one row: lit, and flickering dim.",
     grid=(2, 1), layout=strip_layout("flicker", "2: acesa / fraca (`lit` usa só a col 0)"), anims=[anim("lit", 0, 1), anim("flicker", 0, 2, 6)])

# ---------------------------------------------------------------- G. intro card
add(id="G1", slug="img_lore4", file="img/lore4.bmp", title="Card de intro \"BEGONE!\" (redesenho obrigatório)",
    style="card", core=True, code="**Sim** — `LoreScene.cpp` carrega `assets/img/lore4.bmp` (usado nos cards 4 e 5).",
    rooms="Intro (antes do título).",
    context="""Card 4 da intro (propaganda gamer, narrador parcial). Texto que aparece com ele: `After a decisive ratio,` / `the GAMERS were victorious.` e, no card 5, `Seven GAMERS sealed the` / `degenerates underground` / `with a magic BARRIER.`

**Por que redesenhar (ART-170):** o card atual usa um **rosto real** (meme) e o **logo Doritos**.

**Composição (manter a do original):** metade esquerda, um **gamer demoníaco** gigante (silhueta com headset, olhos brilhando) rindo vitorioso, com um saco de salgadinho **sem marca** em primeiro plano; metade direita, **furries** em fursuit recuando/caindo; a palavra **\"BEGONE!\"** em diagonal cruzando o centro.""",
    single=(398, 214),
    fmt_notes=["**BMP 24-bit, sem alpha, 398×214** — troca direta, sem mexer no código.",
               "Alvo futuro (ART-041): PNG 400×220 para todos os cards; exige mudar o caminho em `LoreScene.cpp`."],
    palette=[("Sépia 1 (mais escuro)", "#2B1608"), ("Sépia 2", "#5A3416"), ("Sépia 3", "#8C5A24"), ("Sépia 4", "#B87D35"),
             ("Sépia 5", "#D9A04A"), ("Sépia 6", "#F0C878"), ("Sépia 7 (mais claro)", "#FFE8B8"), ("Vermelho (só no BEGONE!, opcional)", "#C0301A")],
    reference="`assets/img/lore4.bmp` (398×214, vermelho, com rosto real + logo — só para composição). Estilo dos vizinhos: `assets/img/lore1.bmp` (monocromático sépia/laranja).",
    prompt="""Pixel art intro story card, 398x214 pixels, limited monochrome sepia-orange palette (#2B1608, #5A3416, #8C5A24, #B87D35, #D9A04A, #F0C878, #FFE8B8). Left half: a towering triumphant cartoon gamer silhouette wearing a headset, glowing eyes, laughing with his mouth wide open, an unbranded bag of chips in the foreground. Right half: a crowd of cartoon furry characters in fursuits recoiling and tumbling backwards. Diagonally across the center, the word "BEGONE!" in big jagged white letters. Original characters only, no real faces, no brand logos, nothing explicit.""",
    cleanup=["Quantize para a paleta.", "Escreva o \"BEGONE!\" à mão se a IA errar.", "Exporte BMP 24-bit sem alpha."],
    accept=["Nenhum rosto real nem logo.", "Combina em tom com `lore1`–`lore9`."])

# ---------------------------------------------------------------- render
def palette_table(p):
    out = "| Uso | Hex |\n| :--- | :--- |\n"
    for u, h in p:
        hx = " / ".join(f"`{x.strip()}`" if x.strip().startswith("#") else x for x in h.replace(" · ", " / ").split(" / "))
        out += f"| {u} | {hx} |\n"
    return out

def split_hex(items):
    out = []
    for label, h in items:
        parts = [p.strip() for p in h.replace(" · ", " / ").replace(" + ", " / ").split(" / ")]
        hexes = [p for p in parts if p.startswith("#") and len(p) >= 7]
        labels = [l.strip() for l in label.split("/")]
        if len(labels) != len(hexes):
            labels = [label] * len(hexes)
        for lab, hx in zip(labels, hexes):
            out.append((lab.split("(")[0].strip(), hx[:7]))
    return out

def fid(a):
    return f"{a['id'][0]}{int(a['id'][1:]):02d}_{a['slug']}"

def package(a):
    tpl = f"templates/{fid(a)}.png"
    card = f"cards/{fid(a)}.png"
    if "cell" in a:
        pivot = "feet or base on the red line" if a["style"] in ("ow", "battle", "prop") else "each drawing centered in its cell"
        tpl_en = (f"layout template: copy its canvas proportions, rows, columns and cells exactly (one drawing per gray cell, white cells empty, {pivot}); "
                  "never draw its grid, gray cells, red marks or labels")
    else:
        tpl_en = "canvas template: copy only its proportions and framing; never draw its gray boxes, guide lines or labels"
    imgs = [("Template da grade/canvas", f"[`{tpl}`]({tpl})", "layout exato: proporção, linhas, colunas, pivô", tpl_en),
            ("Cartão de estilo", f"[`{card}`]({card})", "densidade de pixel (âncora), paleta, tamanho e regras numa imagem",
             "style card: match its pixel density (anchor sprite), palette, size and rules; never draw the card itself"
             + ("" if a["id"] in ("A1", "A2") else "; the frog on it is only a pixel-density example, do not draw a frog or copy its features"))]
    if a["id"] in DEPENDS:
        dep = BY_ID[DEPENDS[a["id"]]]
        imgs.append((f"Saída aprovada da ficha {dep['id']}", f"`assets/{dep['file']}` (depois de aprovada)", "mesmo personagem / mesmo design",
                     "the approved design of the same character: keep its design exactly"))
    extras = list(REFS.get(a["id"], []))
    while len(imgs) < 4 and extras:
        label, where, use_pt, en = extras.pop(0)
        imgs.append((label, where, use_pt, f"reference for {en}"))
    return imgs, extras

def build_prompt(a, imgs):
    legend = f"Attached: the spec file {fid(a)}.md (full details) and these images, in order:\n"
    legend += "\n".join(f"- image {i}: {en}" for i, (_, _, _, en) in enumerate(imgs, start=1))
    legend += "\nDraw a new image; never copy the reference images."
    pal = ", ".join(f"{hx} ({lab})" for lab, hx in split_hex(a["palette"]))
    if a["style"] == "card":
        bg = "a full scene with no transparency"
    else:
        bg = "a flat, exact #FF00FF magenta background with no noise"
    if "files" in a:
        out = f"Output: one image with {len(a['files'])} portraits side by side, framed like image 1, on {bg}."
    elif "single" in a:
        out = f"Output: one image with aspect ratio {a['single'][0]}:{a['single'][1]}, on {bg}."
    else:
        out = f"Output: one image with the whole sheet laid out exactly like image 1 ({a['grid'][0]} columns x {a['grid'][1]} rows of equal cells), on {bg}."
    return (RULES_EN + "\n\n" + legend + "\n\n" + STYLE[a["style"]] + " " + a["prompt"] + "\n\n"
            + "Use only these colors: " + pal + ".\n" + out + "\nAvoid: " + NEGATIVE + ".")

def render(a):
    L = []
    L.append(f"# {a['id']} · `assets/{a['file']}` — {a['title']}\n")
    L.append("> Ficha autocontida: tudo o que é preciso para gerar e entregar esta imagem. Índice e regras gerais em [README.md](README.md).\n")
    L.append("| Campo | Valor |\n| :--- | :--- |")
    L.append(f"| Arquivo final | `assets/{a['file']}` |")
    if a.get("anims"):
        L.append(f"| JSON | `assets/{a['file'].rsplit('.', 1)[0]}.json` |")
    L.append(f"| Prioridade | P0 — Ruins{' · **núcleo da vertical slice**' if a['core'] else ''} |")
    L.append(f"| Onde aparece | {a['rooms']} |")
    L.append(f"| O código já lê? | {a['code']} |\n")

    imgs0, _ = package(a)
    names = [f"`{w.split('`')[1]}`" if w.startswith(("[`", "`")) else f"*{l}*" for l, w, _, _ in imgs0]
    L.append("## Envio (resumo)\n")
    L.append(f"Conversa nova no gerador → enviar **{1 + len(imgs0)} anexos**: `{fid(a)}.md` + " + " + ".join(names)
             + " → colar o prompt da seção 7. Detalhes de cada anexo na seção 6.\n")

    L.append("## 1. Contexto\n")
    L.append(a["context"] + "\n")

    L.append("## 2. Regras que valem para esta imagem\n")
    L.append(GLOBAL_RULES)
    for r in STYLE_RULES[a["style"]]:
        L.append(f"- {r}")
    L.append("")

    L.append("## 3. Formato de output esperado\n")
    if "files" in a:
        w, h = a["single"]
        L.append(f"- **Tipo:** PNG RGBA 32-bit, fundo transparente.\n- **Tamanho de cada arquivo:** {w}×{h} px (1× nativo; o jogo mostra a {w*2}×{h*2}).\n- **Arquivos a entregar:**\n")
        L.append("| Arquivo | Expressão |\n| :--- | :--- |")
        for f, d in a["files"]:
            L.append(f"| `assets/portraits/{f}` | {d} |")
        L.append("")
    elif "single" in a:
        w, h = a["single"]
        kind = "BMP 24-bit (sem alpha)" if a["file"].endswith(".bmp") else "PNG RGBA 32-bit, fundo transparente"
        L.append(f"- **Tipo:** {kind}.\n- **Tamanho:** {w}×{h} px (1× nativo; o jogo mostra a {w*2}×{h*2}).\n- **Sheet:** não — imagem única.")
    else:
        (cw, ch), (cols, rows) = a["cell"], a["grid"]
        L.append(f"- **Tipo:** PNG RGBA 32-bit, fundo transparente.")
        L.append(f"- **Célula (um frame):** {cw}×{ch} px.")
        L.append(f"- **Grade:** {cols} coluna(s) × {rows} linha(s).")
        L.append(f"- **Tamanho total do PNG:** **{cols*cw}×{rows*ch} px** (1× nativo; o jogo desenha a 2×).")
        if a.get("origin"):
            L.append(f"- **Pivô (`origin`):** [{a['origin'][0]}, {a['origin'][1]}] dentro de cada célula.")
    for n in a.get("fmt_notes", []):
        L.append(f"- {n}")
    L.append("- **Do gerador de imagem:** peça em alta resolução com fundo `#FF00FF`; o arquivo entregue é o pequeno, já reduzido e recortado (ver \"Do gerador ao arquivo final\").\n")
    if a.get("layout"):
        L.append("**Montagem do sheet:**\n")
        L.append(a["layout"])

    L.append("## 4. Paleta\n")
    L.append(palette_table(a["palette"]))

    L.append("## 5. Referência atual\n")
    L.append(a["reference"] + "\n")

    imgs, alts = package(a)
    L.append("## 6. Pacote de envio (até 5 anexos)\n")
    L.append("Numa conversa nova, anexe **nesta ordem** e cole o prompt da seção 7:\n")
    L.append("| # | Anexo | Onde está | Para quê |\n| :---: | :--- | :--- | :--- |")
    L.append(f"| md | Esta ficha | [`{fid(a)}.md`]({fid(a)}.md) | especificação completa |")
    for i, (label, where, use_pt, _) in enumerate(imgs, start=1):
        L.append(f"| {i} | {label} | {where} | {use_pt} |")
    L.append("")
    if alts:
        L.append("**Alternativas** (troque uma imagem acima, ou use como 5ª se o gerador não contar o `.md`):\n")
        for label, where, use_pt, _ in alts:
            L.append(f"- {label} — {where} — {use_pt}")
        L.append("")
    L.append("Referências são **só entrada**: material do Undertale ou de terceiros fica em `assets/_ref/` (git-ignored) e nunca é commitado nem copiado pixel a pixel.\n")

    L.append("## 7. Prompt (autocontido)\n")
    L.append("Cole inteiro. Ele repete as regras de estilo, a paleta e o formato, e explica cada anexo. Se não enviar alguma imagem, apague a linha dela em \"Attached\".\n")
    L.append("```text\n" + build_prompt(a, imgs) + "\n```\n")

    n = 8
    if a.get("anims"):
        L.append(f"## {n}. JSON\n")
        img = a["file"].rsplit("/", 1)[-1]
        origin = a.get("origin") or (0, 0)
        L.append(f"Salvar como `assets/{a['file'].rsplit('.', 1)[0]}.json`. `frames` padrão 1, `fps` padrão 4, `loop` padrão `true`; animação sem loop congela no último frame; anim inexistente cai em `idle`.\n")
        L.append("```json\n" + json_for(img, a["cell"], origin, a["anims"]) + "\n```\n")
        n += 1

    L.append(f"## {n}. Do gerador ao arquivo final\n")
    L.append(PIPELINE + "\n")
    if a.get("cleanup"):
        L.append("**Específico desta imagem:**\n")
        for c in a["cleanup"]:
            L.append(f"- {c}")
        L.append("")
    n += 1

    L.append(f"## {n}. Checklist de aceite\n")
    if "files" in a:
        dims = f"Cada arquivo tem exatamente {a['single'][0]}×{a['single'][1]} px"
    elif "single" in a:
        dims = f"O arquivo tem exatamente {a['single'][0]}×{a['single'][1]} px"
    else:
        dims = f"O PNG tem exatamente {a['grid'][0]*a['cell'][0]}×{a['grid'][1]*a['cell'][1]} px"
    checks = [dims, "Sem anti-aliasing e sem cores fora da paleta da seção 4",
              "Fundo transparente (magenta removido)" if not a["file"].endswith(".bmp") else "Sem canal alpha (BMP 24-bit)"]
    if a.get("anims"):
        checks.append("JSON válido ao lado do PNG, com os nomes de animação da seção 3")
    checks += a.get("accept", [])
    checks.append("Registrado em `assets/PLACEHOLDERS.md` se não for a arte final")
    for c in checks:
        L.append(f"- [ ] {c}")
    L.append("")
    return "\n".join(L)

BY_ID = {a["id"]: a for a in ASSETS}
index_rows = []
for a in ASSETS:
    name = fid(a) + ".md"
    with open(os.path.join(OUT, name), "w", encoding="utf-8", newline="\n") as f:
        f.write(render(a))
    kit.template(a, os.path.join(OUT, "templates", fid(a) + ".png"))
    kit.card(a, os.path.join(OUT, "cards", fid(a) + ".png"), split_hex(a["palette"]))
    if "files" in a:
        size = f"{len(a['files'])} × {a['single'][0]}×{a['single'][1]}"
    elif "single" in a:
        size = f"{a['single'][0]}×{a['single'][1]}"
    else:
        size = f"{a['grid'][0]*a['cell'][0]}×{a['grid'][1]*a['cell'][1]} (célula {a['cell'][0]}×{a['cell'][1]})"
    reads = "sim" if a["code"].startswith("**Sim") else "não"
    index_rows.append(f"| {a['id']} | [{a['title']}]({name}) | `assets/{a['file']}` | {size} | {reads} | {'★' if a['core'] else ''} |")

SECTIONS = {"A": "Personagens — overworld", "B": "Personagens — batalha", "C": "Retratos de diálogo", "D": "Projéteis",
            "E": "Interface", "F": "Tiles e props", "G": "Card de intro"}
readme = ["# Ruins — prompts de arte (P0)\n",
          "Uma ficha por imagem que o level inicial (Ruins, salas R1–R13) precisa. Cada ficha é **autocontida**: no topo diz exatamente o que enviar ao gerador (o `.md` + até 4 imagens = 5 anexos) e traz contexto, regras, formato de output, paleta, referências, um prompt autocontido, JSON e checklist de aceite. Passo a passo em [WORKFLOW.md](WORKFLOW.md).\n",
          "Fontes: `docs/ASSETS_NEEDED.md` §4.1–§4.7, `docs/LORE.md` (§9, §10, §12, §18.1, §19, §25) e o loader de sprite sheets (`include/core/SpriteSheet.hpp`, `assets/sprites/README.md`). Áudio não entra aqui.\n",
          "**★ = núcleo da vertical slice** (ASSETS_NEEDED §7). **O código já lê** hoje só `sprites/ow/pepe_frog`, `sprites/battle/barky` e `img/lore4.bmp`; o resto já está no formato do loader e aparece quando as cenas forem ligadas a ele.\n",
          "## Kit de identidade visual\n",
          "Cada ficha usa duas imagens próprias: `templates/<ficha>.png` (grade/canvas exato) e `cards/<ficha>.png` (cartão de estilo: densidade de pixel do âncora + paleta da ficha + tamanho + regras, tudo numa imagem para caber no limite de 5 anexos). As imagens gerais abaixo são para consulta e servem de referência alternativa.\n",
          "| Arquivo | Para que serve |\n| :--- | :--- |"] + [f"| [`{f}`]({f}) | {d} |" for f, d in KIT_FILES] + [
          "| `templates/<ficha>.png` | grade/canvas exato de cada ficha (anexo 1) |",
          "| `cards/<ficha>.png` | cartão de estilo de cada ficha (anexo 2) |\n",
          "**Fluxo:** abrir a ficha → bloco **Envio** no topo → conversa nova com o `.md` + as imagens listadas → colar o prompt da seção 7 → pós-processar (seção \"Do gerador ao arquivo final\"). Detalhes em [WORKFLOW.md](WORKFLOW.md).\n",
          "Para regerar fichas, templates e kit depois de mudar algo: `python tools/art_prompts/gen.py` (na raiz do repositório).\n",
          "## Regras gerais\n", GLOBAL_RULES + "\n",
          "## Problemas encontrados nas referências atuais\n",
          "| Arquivo | Problema | Ficha |\n| :--- | :--- | :--- |",
          "| `assets/img/lore4.bmp` | Rosto real (meme) e logo Doritos | G1 |",
          "| `assets/img/test.bmp` | É a foto \"Bliss\" do Windows XP (copyright), usada como fundo do overworld | F1 substitui |",
          "| `assets/img/pepe_char.bmp` | Rosto simplificado (sem pálpebras nem dois lábios) | A1 segue o meme: camiseta azul + traços do rosto (já é o canon na LORE) |",
          "| `assets/sprites/player.png` | 41×43, um traço preto sem relação com o Pepe | ignorar / remover |\n"]
cur = None
for a, row in zip(ASSETS, index_rows):
    sec = a["id"][0]
    if sec != cur:
        cur = sec
        readme.append(f"\n## {sec}. {SECTIONS[sec]}\n")
        readme.append("| # | Ficha | Arquivo final | Tamanho do PNG | Código lê? | Núcleo |\n| :---: | :--- | :--- | :--- | :---: | :---: |")
    readme.append(row)
readme.append("\n## Depois de gerar\n")
readme.append("1. Coloque o PNG (e o JSON) no caminho final em `assets/`.\n2. Registre em `assets/PLACEHOLDERS.md` (fonte/gerador, autor, licença). Para imagens de IA, anote o serviço e confira os termos de uso comercial.\n3. Teste o que o código já lê: `pepe_frog` (andar nas 4 direções, pivô nos pés), `battle/barky` (idle, hurt, blush após Cringe Check, spared) e o card `lore4`.\n")
with open(os.path.join(OUT, "README.md"), "w", encoding="utf-8", newline="\n") as f:
    f.write("\n".join(readme))

# ---------------------------------------------------------------- send checklist
def short_ref(label, where):
    if where.startswith(("[`", "`")):
        return f"`{where.split('`')[1]}`"
    return label.replace("Foto/ilustração real: ", "foto: ").replace("Undertale — ", "Undertale: ")

envios = ["# O que enviar em cada ficha\n",
          "Toda chamada = **conversa nova** no gerador + anexos abaixo (`templates/`, `cards/` e `style/` ficam nesta pasta; `assets/...` é a partir da raiz do repositório) + **prompt da seção 7** da ficha.\n",
          "- **Obrigatórios (sempre 3):** o `.md` da ficha, `templates/<ficha>.png` e `cards/<ficha>.png`.",
          "- **Recomendados (até 2):** as colunas 4 e 5. `assets/...` = saída já aprovada de outra ficha (gere aquela antes); `style/...` = imagem do kit; \"Undertale: …\" = sprite do jogo original (The Spriters Resource, só como referência, guardado em `assets/_ref/`); \"foto: …\" = busca de imagem com esse texto.",
          "- Se não enviar a 4 ou a 5, apague a linha dela em \"Attached\" no prompt.\n"]
for sec in SECTIONS:
    rows = [a for a in ASSETS if a["id"][0] == sec]
    envios.append(f"\n## {sec}. {SECTIONS[sec]}\n")
    envios.append("| ✓ | Ficha (1 · 2 · 3 = md, template, card) | 4 | 5 |\n| :---: | :--- | :--- | :--- |")
    for a in rows:
        imgs, _ = package(a)
        extra = [short_ref(l, w) for l, w, _, _ in imgs[2:]] + ["—", "—"]
        envios.append(f"| [ ] | [`{fid(a)}`]({fid(a)}.md) | {extra[0]} | {extra[1]} |")
with open(os.path.join(OUT, "ENVIOS.md"), "w", encoding="utf-8", newline="\n") as f:
    f.write("\n".join(envios) + "\n")

# ---------------------------------------------------------------- identity kit

by_id = {a["id"]: a for a in ASSETS}
palette_sections = [
    ("RUINS - overworld area", split_hex(RUINS_PALETTE)),
    ("BATTLE - monochrome (+ blush, heal)", [("line", "#FFFFFF"), ("fill", "#000000"), ("blush", "#FF77C8"), ("heal", "#00FF00")]),
    ("UI - buttons", [("normal", "#FF7F27"), ("selected", "#FFFF00"), ("locked", "#808080")]),
    ("SOULS", split_hex(by_id["E2"]["palette"])),
    ("PEPE", split_hex(by_id["A1"]["palette"])),
    ("FURIELL", split_hex(by_id["A3"]["palette"])),
    ("BARKY", split_hex(by_id["A4"]["palette"])),
    ("FLOOFY", split_hex(by_id["A5"]["palette"])),
    ("SAVE POINT", split_hex(by_id["E10"]["palette"])),
    ("INTRO CARDS - sepia", split_hex(by_id["G1"]["palette"])),
]
kit.img_palettes(palette_sections, os.path.join(OUT, "style", "01_palettes.png"))
kit.img_rules(os.path.join(OUT, "style", "02_rendering_rules.png"))
kit.img_scale(os.path.join(OUT, "style", "03_scale_lineup.png"))
kit.img_anchor(os.path.join(OUT, "style", "04_pepe_anchor.png"), os.path.join(OUT, "style", "04_pepe_anchor_native.png"))
kit.img_legacy(os.path.join("assets", "img", "pepe_char.bmp"), os.path.join(OUT, "style", "05_pepe_legacy.png"))
with open(os.path.join(OUT, "STYLE_GUIDE.md"), "w", encoding="utf-8", newline="\n") as f:
    f.write(style_guide(palette_sections, KIT_FILES))
order_rows = [f"| {DEPENDS[k]} — {BY_ID[DEPENDS[k]]['title']} | {k} — {BY_ID[k]['title']} |" for k in DEPENDS]
with open(os.path.join(OUT, "WORKFLOW.md"), "w", encoding="utf-8", newline="\n") as f:
    f.write(workflow(order_rows))
print(len(ASSETS), "fichas + kit")
