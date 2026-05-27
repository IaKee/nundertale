# Como gerar cada imagem (sem projeto, até 5 anexos por chamada)

Cada ficha (`A01_…md`, `B01_…md`…) tem no topo um bloco **Envio** com exatamente o que mandar numa conversa nova do gerador:

1. **O arquivo `.md` da ficha** (especificação completa).
2. **Até 4 imagens**, na ordem da ficha: o template da grade, o cartão de estilo da ficha (densidade de pixel + paleta + tamanho + regras numa imagem só) e 1–2 referências de identidade.
3. **O prompt da seção 7**, colado inteiro. Ele é autocontido: repete as regras de estilo, a paleta, o formato e explica o papel de cada anexo.

Se o seu gerador não contar o `.md` como referência, dá para trocar o `.md` pela 5ª imagem listada em "Alternativas" da ficha (o prompt já contém tudo do `.md` que importa para a imagem).

## Dicas

- Personagens: peça primeiro **só o frame parado de frente** (acrescente "Only draw the front idle frame, alone, at the size of one cell"), aprove e depois peça o sheet inteiro, trocando a última referência pelo frame aprovado.
- Fundo: o gerador costuma entregar um magenta com ruído (`#EF0DF0` etc.). Recorte com tolerância (R>170, G<110, B>170) e limpe a franja rosada do contorno.
- A saída sai grande (~9–10 px de tela por pixel de arte). Reduza com o pixel-âncora como medida, encaixe na paleta e retoque os perfis à mão.

## Ordem recomendada (as saídas aprovadas viram referência de outras fichas)

| Primeiro | Depois (usa a saída aprovada como referência) |
| :--- | :--- |
| A1 — Pepe (protagonista) — overworld | A2 — Pepe deitado / levantando (R1) |
| A4 — Barky — overworld | B1 — Barky — batalha (mini-boss) |
| A3 — Furiell — overworld | B2 — Furiell — batalha (boss) |
| A5 — Floofy — overworld | B3 — Floofy — batalha (tutorial) |
| B2 — Furiell — batalha (boss) | C1 — Retratos de diálogo — Furiell |
| B3 — Floofy — batalha (tutorial) | C2 — Retratos de diálogo — Floofy |
| B1 — Barky — batalha (mini-boss) | C3 — Retratos de diálogo — Barky |

## Arquivos de apoio

| Pasta | Conteúdo |
| :--- | :--- |
| `templates/` | grade/canvas exato de cada ficha (anexo 1) |
| `cards/` | cartão de estilo de cada ficha (anexo 2) |
| `style/` | imagens gerais (paletas, regras, escala, âncora, Pepe legado) — para consulta e como referência alternativa |
| `STYLE_GUIDE.md` | guia de identidade visual completo (para você; não precisa enviar) |
