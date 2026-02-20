# nUndertale — Backlog de fidelidade 1:1 ao Undertale (visual, gameplay e feeling) com o roteiro próprio

> Gerado em 2026-10-16 a partir da leitura completa do código C++20/Raylib atual (`src/`, `include/`), do `ROTEIRO.md`, do `README.md` e da comparação entre o print atual do jogo e um print de batalha do Undertale (Loox, Vegetoid e Migosp).
>
> **Objetivo:** a paródia deve ficar praticamente indistinguível do Undertale em layout, proporções, timings, sons-chave e "sensação de controle", trocando apenas o **conteúdo** (personagens, textos, piadas, músicas e sprites originais) pelo roteiro do nUndertale.
>
> **Nada deste backlog foi implementado.** Cada item é autocontido: descreve o que fazer, por que, onde mexer e como saber que terminou.

---

## Sumário

0. [Como usar este backlog](#0-como-usar-este-backlog)
1. [Diagnóstico: print atual vs Undertale](#1-diagnóstico-print-atual-vs-undertale)
2. [Ficha técnica de referência (medidas do Undertale)](#2-ficha-técnica-de-referência-medidas-do-undertale)
3. [Decisões pendentes (bloqueiam itens)](#3-decisões-pendentes-bloqueiam-itens)
4. [Bugs encontrados no código atual](#4-bugs-encontrados-no-código-atual)
5. [Épicos e itens](#5-épicos-e-itens)
   - [REPO — Higiene do repositório e build](#repo--higiene-do-repositório-e-build)
   - [CORE — Engine, loop e cenas](#core--engine-loop-e-cenas)
   - [GFX — Renderização, fontes e estilo visual](#gfx--renderização-fontes-e-estilo-visual)
   - [TXT — Sistema de texto e diálogo](#txt--sistema-de-texto-e-diálogo)
   - [BUI — Batalha: layout e UI](#bui--batalha-layout-e-ui)
   - [BFL — Batalha: fluxo e máquina de estados](#bfl--batalha-fluxo-e-máquina-de-estados)
   - [FGT — Batalha: FIGHT](#fgt--batalha-fight)
   - [ACT — Batalha: ACT](#act--batalha-act)
   - [ITM — Batalha: ITEM](#itm--batalha-item)
   - [MRC — Batalha: MERCY / Fuga](#mrc--batalha-mercy--fuga)
   - [BHL — Batalha: turno inimigo e bullet hell](#bhl--batalha-turno-inimigo-e-bullet-hell)
   - [BND — Batalha: finais (vitória, poeira, level up, game over)](#bnd--batalha-finais-vitória-poeira-level-up-game-over)
   - [ENC — Encontros e transição para batalha](#enc--encontros-e-transição-para-batalha)
   - [OVW — Overworld](#ovw--overworld)
   - [MNU — Menu do overworld (ITEM / STAT / CELL)](#mnu--menu-do-overworld-item--stat--cell)
   - [SAV — Save point, save/load e meta-persistência](#sav--save-point-saveload-e-meta-persistência)
   - [PRG — Progressão, equipamento, economia e rotas](#prg--progressão-equipamento-economia-e-rotas)
   - [FRT — Front-end (disclaimer, intro, título, nome, settings)](#frt--front-end-disclaimer-intro-título-nome-settings)
   - [AUD — Áudio](#aud--áudio)
   - [FUR — Mecânica exclusiva: Furrificação](#fur--mecânica-exclusiva-furrificação)
   - [NAR — Roteiro e escrita](#nar--roteiro-e-escrita)
   - [C1 — Conteúdo: Capítulo 1 "A Descida ao Mt. Dew" (Ruínas)](#c1--conteúdo-capítulo-1-a-descida-ao-mt-dew-ruínas)
   - [C2 — Conteúdo: Snowdinha](#c2--conteúdo-snowdinha)
   - [C3+ — Conteúdo: áreas futuras](#c3--conteúdo-áreas-futuras)
   - [ART — Arte (lista de assets)](#art--arte-lista-de-assets)
   - [L10N — Localização](#l10n--localização)
   - [QA — Ferramentas de debug, testes e CI](#qa--ferramentas-de-debug-testes-e-ci)
   - [DST — Distribuição](#dst--distribuição)
6. [Roadmap por marcos](#6-roadmap-por-marcos)
7. [Apêndices](#7-apêndices)

---

## 0. Como usar este backlog

### Formato de cada item

```
### <ID> — <título>
Prioridade · Tamanho · Depende de
O quê / Por quê / Detalhes / Onde mexer / Critério de aceite
```

| Campo | Valores |
| :--- | :--- |
| **Prioridade** | **P0** = sem isso não parece Undertale (bloqueia o vertical slice) · **P1** = fidelidade importante · **P2** = polimento/feeling fino · **P3** = desejável/longo prazo |
| **Tamanho** | **P** ≤ 2h · **M** ≈ meio dia a 1 dia · **G** 2–4 dias · **GG** ≥ 1 semana (quebrar antes de começar) |
| **Status** | `[ ]` a fazer · `[~]` em andamento · `[x]` feito · `[!]` bloqueado (anotar motivo) |

### Regras gerais para quem implementar

1. **Medidas em pixels são do canvas virtual 640×480** (`VIRTUAL_WIDTH/HEIGHT` em `include/core/Constants.hpp`), que é a resolução real do Undertale.
2. **Toda pixel art é desenhada em escala inteira 2×** (sprite nativo 20×20 vira 40×40 na tela). Nenhum personagem, bala ou ícone deve ser feito com primitivas (`DrawCircle`, `DrawTriangle`) na versão final — primitivas só como placeholder de debug.
3. **Assets do Undertale não podem ser copiados** (sprites, músicas, sons e fontes proprietárias). "1:1" significa **mesmo layout, proporção, timing e sensação**, com arte e som originais feitos no mesmo estilo. Ver `D-04`.
4. Valores marcados com **(confirmar)** vêm de memória/documentação da comunidade e devem ser conferidos contra gameplay de referência (gravação a 30 fps, quadro a quadro) antes de virar constante.
5. Ao concluir um item, marque `[x]`, anote o commit/PR e atualize o `README.md` se a funcionalidade for visível ao jogador.

---

## 1. Diagnóstico: print atual vs Undertale

Comparação direta dos dois prints (o atual é a luta com Barky; o de referência é "Loox and co. decided to pick on you!").

| # | Elemento | Undertale (referência) | nUndertale hoje | Itens |
| :-- | :--- | :--- | :--- | :--- |
| 1 | **Fundo da batalha** | Grade verde de linhas de 1 px (#27AF4B) cobrindo a área de cima | Preto liso | `BUI-002` |
| 2 | **Sprite do inimigo** | Pixel art branca e monocromática sobre preto, 2×, pés em y≈241, com animação idle | Primitivas cinza/azuladas (círculo, triângulos, retângulo), estático | `ART-010`, `BUI-003` |
| 3 | **Nome/HP do inimigo** | Não aparecem permanentemente; nome só na lista de alvos (com barra de HP no FIGHT) | "Barky HP: 30 / 30" fixo embaixo do inimigo | `BUI-004` |
| 4 | **Caixa de texto** | 575×140 px, de (32,250) a (606,389), borda branca de 5 px, ocupa a largura toda | 200×130 centralizada, borda de 4 px | `BUI-001` |
| 5 | **Texto da caixa** | Fonte mono pixelada grande (~32 px de linha), "* " com recuo deslocado na 2ª linha, digitado com som | Fonte pequena, texto vaza da borda (sem quebra de linha) | `TXT-001`, `TXT-002`, `GFX-003` |
| 6 | **HUD** | `NOME  LV n  HP [barra] x / y` com fontes distintas, label "HP" minúsculo, barra de largura = HPmax×1,2 | Tudo na mesma fonte, "FURRY: 0%" no meio, barra fixa de 60 px em x=320 que pode sobrepor o texto | `BUI-005`, `FUR-003` |
| 7 | **Botões** | 4 sprites 110×42 com ícone + texto (espada, coração, bolsa, X), laranja #FF7F27; selecionado vira amarelo #FFFF00 e o ícone vira a alma vermelha | Retângulos com borda e texto, laranja #FFA500 (claro demais), cursor é um ponto verde dentro do botão | `BUI-006`, `ART-020` |
| 8 | **Posição dos botões** | x = 32, 185, 345, 500; y = 432 | x = 60, 200, 340, 480; y = 418 | `BUI-006` |
| 9 | **Alma** | Coração vermelho 16×16 em sprite | Coração verde montado com círculos + triângulo | `BUI-007`, `D-01` |
| 10 | **Frame rate / sensação** | Lógica e render a 30 fps, movimento em px/quadro | 60 fps com `dt` variável | `CORE-001` |
| 11 | **Encontros múltiplos** | Até 3 inimigos ("X and co.") | 1 inimigo fixo | `BFL-004` |
| 12 | **Fluxo do turno** | Texto → escolha → bolha de fala do inimigo → caixa encolhe → ataque → caixa volta → texto de "flavor" | Mensagem → turno inimigo direto, com bolha sobreposta, caixa nunca muda de tamanho | `BFL-001`..`BFL-003`, `BHL-001` |
| 13 | **Overworld** | Sem HUD, câmera seguindo, salas maiores que a tela, menu C | HUD fixo na parte de baixo com dicas de tecla, sala única do tamanho da tela | `OVW-001`, `OVW-002`, `MNU-*` |

---

## 2. Ficha técnica de referência (medidas do Undertale)

> Medido pixel a pixel no print de referência 640×480 (Loox/Vegetoid/Migosp). Valores sem "(confirmar)" foram **medidos**.

### 2.1 Batalha — geometria

| Elemento | Valor |
| :--- | :--- |
| Caixa (estado texto/menu), borda externa | x 32→606, y 250→389 (575×140) |
| Espessura da borda da caixa | 5 px, branco (#FFFFFF) |
| Interior útil da caixa | x 37→601, y 255→384 |
| Texto: início da 1ª linha | x = 52 (o "*"), topo do glifo em y ≈ 276 |
| Altura de linha | 32 px (linhas em y≈276 e y≈308) |
| Recuo da 2ª linha | alinhado ao primeiro caractere depois de "* " (recuo deslocado / hanging indent) |
| Grade de fundo: linhas horizontais | y = 9, 126, 244 (1 px) |
| Grade de fundo: linhas verticais | x = 15, 115, 218, 317, 420, 523, 623 (1 px) |
| Cor da grade | #27AF4B (39,175,75) |
| Sprites inimigos | pés em y ≈ 241, topo de um inimigo médio em y ≈ 126 (≈115 px de altura em 2×) |
| HUD: linha de base | glifos de y ≈ 400 a 419 |
| HUD: nome | x = 30 |
| HUD: "LV" | x = 132 (número em x ≈ 171) |
| HUD: label "HP" | x = 244→265 (fonte pequena, sprite) |
| HUD: barra de HP | x = 275, y = 400, altura 20 px, largura = `HPmax × 1,2` (24 HP → 29 px) |
| HUD: barra — cores | fundo vermelho #FF0000, preenchimento amarelo #FFFF00 proporcional ao HP atual |
| HUD: números "16 / 24" | começam ≈ 18 px depois do fim da barra (x ≈ 322 para HPmax 24) |
| Botões (4) | x = 32, 185, 345, 500 · y = 432 · 110×42 · borda 2 px |
| Botões — cores | normal #FF7F27 (medido #FF8124) · selecionado #FFFF00 |
| Botão selecionado | o ícone (espada/coração/bolsa/X) é substituído pela alma vermelha |

### 2.2 Batalha — comportamento (confirmar)

| Elemento | Valor de referência |
| :--- | :--- |
| Frame rate | 30 fps (lógica e desenho) |
| Alma | 16×16, vermelho #FF0000, velocidade 4 px/quadro (120 px/s) (confirmar), hitbox menor que o sprite (~8×8 central) (confirmar) |
| Invencibilidade após dano | ~30 quadros (1 s) com piscar (confirmar), aumenta com certas armaduras |
| Caixa do ataque padrão | ~165×140 centralizada em x, base alinhada à base da caixa grande (confirmar por padrão de ataque) |
| Redimensionamento da caixa | animado, ~6–10 quadros, interpolação linear das 4 bordas (confirmar) |
| Fórmula de dano do jogador (comunidade) | `dano = round((AT + AT_arma − DF_inimigo + rand(0..2)) × mult)`; `mult = 2,2` se `|distância ao centro| ≤ 12`, senão `(1 − |distância|/273) × 2` (confirmar) |
| HP máximo por LV | `HPmax = 16 + 4·LV` (LV1 = 20 … LV19 = 92, LV20 = 99) |
| Tabela de EXP para subir de LV | 10, 30, 70, 120, 200, 300, 500, 800, 1200, 1700, 2500, 3500, 5000, 7000, 10000, 15000, 25000, 50000, 99999 |
| Texto de batalha | 1 caractere/quadro (30 cps) com som de "voz" por caractere (confirmar) |

### 2.3 Fontes usadas pelo Undertale (para escolher equivalentes)

| Uso | Fonte original | Equivalente sugerido |
| :--- | :--- | :--- |
| Texto principal (caixas, menus) | DTM-Mono ("Determination Mono") | **Determination Mono** (fan font livre; conferir licença) ou manter `8bitoperator.ttf` como reserva |
| HUD (nome, LV, números) | "Mars Needs Cunnilingus" (pixel) | conferir licença; alternativa: bitmap font própria 2× |
| Números de dano | "Hachicro" | bitmap font própria (algarismos 0–9 + "MISS") |
| Label "HP"/"KR", textos minúsculos | sprites / "Crypt of Tomorrow" | sprite próprio |
| Bolha de fala do inimigo | fonte pequena estilo DotumChe (confirmar) | pixel font pequena ~12 px nativa |
| Título | Monster Friend | `monster_friend.otf` / `monster_friend_fore.otf` (já no repo) |
| Senpai Sans | Comic Sans | fonte "comic" livre (ex.: Comic Neue) em minúsculas |
| Papyrus Streamer | Papyrus | fonte livre de estilo papiro, sempre em MAIÚSCULAS |

---

## 3. Decisões pendentes (bloqueiam itens)

Cada decisão tem uma **recomendação**. Registrar a escolha aqui (e no `ROTEIRO.md` quando for narrativa) antes de implementar os itens dependentes.

### D-01 — Cor da alma do Pepe
- **Contexto:** hoje a alma é verde ("frog soul"). No Undertale, verde significa o modo escudo (luta da Undyne); vermelho é a alma padrão.
- **Opções:** (a) vermelha padrão, como no Undertale; (b) verde permanente; (c) vermelha, com verde como piada/easter egg (ex.: nome `PEPE` ou `FROG`).
- **Recomendação:** **(c)**. Mantém o 1:1 e libera as cores para os modos de alma (azul/verde/amarelo/roxo — `BHL-010`..`BHL-013`).
- **Bloqueia:** `BUI-007`, `BHL-010`+.

### D-02 — Frame rate da lógica
- **Recomendação:** lógica em **30 Hz fixos** (acumulador), render a 30 fps. Todas as constantes passam a ser "por quadro", o que permite transcrever valores do Undertale direto. Opção de interpolação a 60 fps fica como P3.
- **Bloqueia:** `CORE-001` e todos os itens de timing.

### D-03 — Idioma principal
- **Contexto:** o código e o roteiro estão em inglês; o usuário fala PT-BR; existe `assets/lang/en.json` (herdado da versão Python).
- **Recomendação:** textos **em inglês como master** (fidelidade ao tom do Undertale) com PT-BR como segunda língua via `L10N-*`. Nenhum texto fica hardcoded no C++.

### D-04 — Licenciamento de assets
- **Contexto:** o repositório (público no GitHub, conta IaKee) contém `assets/music/Undertale - Once Upon A Time Remix (ft Yuri, zR).ogg` e sons de origem desconhecida (`u1..u12.ogg`, `sans_type.wav`).
- **Recomendação:** criar `assets/LICENSES.md` listando origem e licença de cada asset; substituir tudo que for ripado ou remix sem licença por composições/sons originais "no estilo" antes de qualquer release público. Ver `AUD-020`, `DST-003`.

### D-05 — Semântica do Medidor Furry (FUR)
- **Contexto:** o ROTEIRO diz que (1) com HP 0 o Pepe não morre, ele se furrifica, e (2) "a cada golpe descuidado" ganha orelhas, patas e rabo. O código soma +8% por golpe e mostra "FURRY: x%" no HUD.
- **Opções:** (a) medidor só dentro da batalha, zera depois; (b) medidor persistente com estágios visuais, reduzido por itens (Duct Tape) e por save point; (c) persistente **e** meta (sobrevive a reload, como as memórias do Flowey).
- **Recomendação:** **(b)**, com elemento de (c) só para o contador de "furrificações completas" (game overs). Detalhes em `FUR-001`.

### D-06 — Chefe final das Ruínas: Barky ou Furiell?
- **Contexto:** o ROTEIRO chama Barky de "Ruins Boss / Mini-boss" e diz que poupá-lo abre o portão para Snowdinha; mas Furiell é a "Guardiã das Ruínas" (paródia da Toriel), sem nenhuma cena escrita.
- **Recomendação:** **Barky = primeiro encontro/mini-chefe** (substitui Froggit/Napstablook como "tutorial de ACT"); **Furiell = chefe final**, na porta de saída, como a Toriel. Exige reescrever a Cena 4 (`NAR-003`).

### D-07 — Nomes das rotas e do LV/EXP
- **Proposta:** Pacifista → **"Wholesome Run"**; Neutra → **"Normie Run"**; Genocida → **"Toxic Gamer Run"**. LV = "Level of Vitriol"; EXP = "eXcessive Posting". Validar no roteiro (`NAR-010`).

### D-08 — Formato dos mapas
- **Recomendação:** **Tiled** (`.tmx` editado, exportado em `.json`), com camadas `ground`, `decor`, `collision` (objetos retângulo/polígono), `triggers`, `npcs`, `spawn`. O loader fica em C++ com um parser JSON header-only (`nlohmann/json` ou `simdjson`), vendorizado.

### D-09 — Scripting de padrões de ataque e cutscenes
- **Opções:** (a) classes C++ por padrão + parâmetros em JSON; (b) Lua embutido.
- **Recomendação:** **(a)** agora (menos dependências); reavaliar Lua em `CORE-020` (P3) quando houver mais de 30 padrões.

### D-10 — Corrida no overworld
- **Contexto:** o README diz "X / Shift = Run". O Undertale **não tem corrida** (isso é coisa do Deltarune).
- **Recomendação:** remover a corrida da versão 1:1, ou deixá-la só como opção de acessibilidade desligada por padrão.

---

## 4. Bugs encontrados no código atual

> Corrigir antes ou junto dos itens de fidelidade, porque vários deles mudam o fluxo da batalha.

| ID | Arquivo | Bug | Correção sugerida |
| :--- | :--- | :--- | :--- |
| **BUG-01** | `src/core/Game.cpp:43` / `SceneManager::pushScene` | A primeira cena entra com `pushScene`, que **não chama `onEnter`**, então `DisclaimerScene::onEnter` (som + reset) nunca roda. | **[x] Corrigido:** `pushScene` e `popScene` agora chamam `onEnter`, `onPause`, `onResume` e `onExit` com gerenciamento completo de ciclo de vida. |
| **BUG-02** | `src/combat/BattleSystem.cpp` (estado `DialogMessage`) | Se o inimigo está poupável depois de um ACT, apertar Z **vence a luta sozinho**, sem usar MERCY → Spare. | **[x] Corrigido:** `DialogMessage` agora só termina a batalha se o inimigo morreu ou foi poupado de fato via MERCY (`m_isSpared`). |
| **BUG-03** | `src/scenes/BattleScene.cpp` | A derrota (`Defeat`) volta para o `OverworldScene` com HP 0; não há game over. | **[x] Corrigido:** transição direta para `GameOverScene` com quebra da alma, determinação e recarregamento do save (`BND-010`). |
| **BUG-04** | `BattleScene.cpp` / `OverworldScene` | Voltar da batalha **recria o `OverworldScene`**: o Pepe volta para a posição inicial, o Barky continua lá e a luta pode se repetir. | **[x] Corrigido:** a batalha é empilhada via `pushScene` e desempilhada via `popScene`, preservando o estado e a posição no overworld. |
| **BUG-05** | `BattleSystem::handleItemInput` | Sempre consome o **último** item, sem lista de seleção; qualquer item que não seja "Mt. Dew" cai no ramo dos Doritos. | Tabela de itens dirigida por dados (`ITM-001`). |
| **BUG-06** | `BattleSystem::handleActInput` | Texto do Check hardcoded ("ATK 5 DEF 2") não bate com `Enemy("Barky", 30, 4, 2)`; os ACTs são comparados por string literal. | ACTs definidos nos dados do inimigo (`ACT-001`). |
| **BUG-07** | `BattleSystem::updateEnemyTurn` | Dano fixo de 3 e +8% de furry; `m_atk`/`m_def` do inimigo nunca são usados. | **[x] Corrigido:** dano por bala + AT do inimigo − DF do jogador (`BHL-005`). |
| **BUG-08** | `BattleSystem::handleFightTargetInput` | Usa `GetFrameTime()` em vez do `dt` recebido. | **[x] Corrigido:** recebe `dt` fixo da lógica de 30 Hz (`CORE-001`). |
| **BUG-09** | `BattleSystem::render` | O texto do narrador não quebra linha e vaza da caixa (visível no print). | `TXT-002`. |
| **BUG-10** | `include/combat/Bullet.hpp` | A colisão usa um AABB pelo `radius` até para a bala de texto "UWU" (que é bem maior); as balas não são removidas ao sair da tela e são desenhadas fora da caixa. | **[x] Corrigido:** hitboxes específicas por tipo (UwU, SodaCan, Bone) + recorte (scissor) no interior da caixa + culling de saída (`BHL-002`, `BHL-003`). |
| **BUG-11** | `src/ui/DialogBox.cpp` | **Z completa o texto** durante a digitação. No Undertale, Z só avança depois que a página termina; **X** é que pula. | **[x] Corrigido:** apenas X (Cancel) pula a digitação; Z avança páginas completas (`TXT-004`). |
| **BUG-12** | `BattleSystem::render` (HUD) | A barra de HP fica fixa em x=320 e se sobrepõe ao texto se o nome for comprido. | **[x] Corrigido:** layout 1:1 nas posições medidas (seção 2.1) com barra proporcional (`BUI-005`). |
| **BUG-13** | `src/services/AudioService.cpp` | O volume mestre é aplicado duas vezes (`SetMasterVolume` + multiplicação em `playSound`). | **[x] Corrigido:** removida multiplicação dupla em `playSound`. |
| **BUG-14** | `OverworldScene::update` | ESC volta ao título na hora, sem confirmação, e perde o progresso. | **[x] Corrigido:** removido retorno imediato; segurar ESC por 1s sai do jogo globalmente com feedback visual (`CORE-006`). |
| **BUG-15** | `ResourceService::getFont` | As fontes são carregadas num tamanho e desenhadas em outro (ex.: carrega 22 e desenha 20), o que deixa os pixels irregulares com filtro POINT. | `GFX-003`. |
| **BUG-16** | `BattleSystem` | EXP é ganha mas o LV nunca sobe; o gold de spare (+10) e de kill (+15) está hardcoded. | **[x] Corrigido:** tabela oficial de LV/EXP adicionada; aumento de VITRIOL, HP recalculado e cura no level up (`PRG-001`). |
| **BUG-17** | `BattleSystem::handleFightTargetInput` | O cursor de ataque sempre começa na esquerda. No Undertale, o lado é aleatório. | **[x] Corrigido:** entrada aleatória pela esquerda ou direita com velocidade direcional correspondente (`FGT-002`). |

---

## 5. Épicos e itens

### REPO — Higiene do repositório e build

#### REPO-000 — Versionar a reescrita C++20 (urgente) `[x]`
P0 · P · REPO-001
- **Situação em 2026-10-16:** o `git status` mostra que **toda a versão moderna está fora do Git**: `src/`, `include/`, `CMakeLists.txt`, `build.bat` e `ROTEIRO.md` aparecem como untracked; os assets foram movidos de `old/assets/` (deleções pendentes) para `assets/` (untracked); o `README.md` está modificado. O único lugar onde esse código existe é a pasta do OneDrive.
- **O quê:** primeiro o REPO-001 (para não versionar `vendor/` de 743 MB nem os `.exe`/`.o`), depois um commit "C++20 + Raylib rewrite" com `src/`, `include/`, os scripts de build, a documentação e o `assets/` reorganizado (a deleção de `old/assets/` + a adição em `assets/` aparecem como rename), e push pela conta pessoal (`/iakee_push`).
- **Aceite:** um clone novo + setup do vendor compila o `nundertale_modern.exe`.

#### REPO-001 — `.gitignore` completo `[x]`
P0 · P · —
- **O quê:** ignorar `vendor/` (743 MB, hoje não versionado mas também não ignorado), `*.exe`, `*.o`, `*.dll` (exceto se a decisão for distribuir), `build/`, `saves/`, `**/__pycache__`, `crash_reports/`, `*.psd` (ou mover para LFS, ver REPO-004).
- **Aceite:** `git status` limpo depois de um build completo.

#### REPO-002 — Remover o legado Python da raiz `[x]`
P1 · P · —
- **O quê:** `main.py`, `CONSTANTS.py`, `controller/`, `model/`, `view/`, `log/`, `test/*.py`, `crash_reports/` e os `__pycache__` são da versão Python antiga. Mover para `old/legacy_python/` (seguindo o padrão de `old/legacy_c/`) ou apagar.
- **Atenção:** a reorganização `old/assets/` → `assets/` ainda não foi commitada (REPO-000). Fazer o REPO-000 antes e não misturar os dois no mesmo commit. Também ficam soltos na raiz `nundertale.c`, `nundertale.exe`, `nundertale.o` e `allegro-5.0.10-monolith-mt.dll`, que são duplicatas de `old/legacy_c/`.
- **Aceite:** a raiz contém só `src/`, `include/`, `assets/`, `data/`, `config/`, `tools/`, a documentação e os scripts de build.

#### REPO-003 — Um único sistema de build `[x]`
P1 · M · —
- **Problema:** o `build.bat` lista os `.cpp` manualmente; o `CMakeLists.txt` usa `GLOB_RECURSE`. Arquivo novo esquecido no `.bat` = erro de link.
- **O quê:** fazer o `build.bat` chamar o CMake (gerador "MinGW Makefiles" com o `w64devkit`) **ou** gerar a lista com `for /r src %%f in (*.cpp)`. Adicionar alvos `debug` (`-O0 -g -DNUNDERTALE_DEBUG`) e `release` (`-O2 -s -mwindows`).
- **Aceite:** `build.bat`, `build.bat debug` e `cmake --build` geram o mesmo executável; criar um `.cpp` novo não exige editar script nenhum.

#### REPO-004 — Fontes de arte fora de `assets/` `[ ]`
P2 · P · —
- **O quê:** mover `assets/Untitled-1.psd`, `assets/lore.psd` e `assets/img/deletethis/` para `art_src/` (com Git LFS) ou apagar. `assets/` passa a conter só o que o jogo carrega.

#### REPO-005 — Script de setup do `vendor/` `[ ]`
P1 · M · REPO-001
- **O quê:** `tools/setup_vendor.ps1`, que baixa o w64devkit e o raylib 6.2 (win64 mingw-w64) com checksum e extrai em `vendor/`. Documentar no README.
- **Aceite:** clone limpo + `tools/setup_vendor.ps1` + `build.bat` = jogo rodando.

#### REPO-006 — Warnings zerados `[ ]`
P2 · P · REPO-003
- **O quê:** compilar com `-Wall -Wextra -Wpedantic -Wshadow -Wconversion` e corrigir (ex.: os parâmetros `context` não usados nos lambdas de `OverworldScene`).

#### REPO-007 — Pasta `data/` para conteúdo `[x]`
P0 · P · —
- **O quê:** criar a estrutura `data/{enemies,encounters,items,rooms,dialogue/<lang>,patterns,shops}` e um `data/README.md` explicando os schemas. Base para ACT-001, ITM-001, OVW-003 e TXT-010.

---

### CORE — Engine, loop e cenas

#### CORE-001 — Loop de lógica fixo a 30 Hz `[x]`
P0 · M · D-02
- **Por quê:** é o que mais muda o "feeling". O Undertale roda a 30 fps; velocidades, durações de i-frame, piscadas e digitação são todas por quadro.
- **O quê:** em `Game::run`, usar um acumulador `while (acc >= 1/30) { update(1/30); acc -= 1/30; }`; `SetTargetFPS(30)` ou render a 60 repetindo o quadro. Trocar `dt` em segundos por `frames` onde fizer sentido (`constexpr int SOUL_SPEED = 4;`).
- **Onde:** `src/core/Game.cpp`, `include/core/Constants.hpp`, todas as cenas.
- **Aceite:** com a janela minimizada ou lenta, a lógica não acelera nem pula colisões; o movimento é idêntico a 30, 60 e 144 Hz.

#### CORE-002 — Pilha de cenas de verdade (push/pop com estado) `[x]`
P0 · M · BUG-01, BUG-04
- **O quê:** `SceneManager` com `push(scene, transition)`, `pop(transition)`, `replace(...)`. A batalha é **empilhada** sobre o overworld (que fica pausado e preservado) e desempilhada no fim. `onPause/onResume` em `IScene`.
- **Aceite:** depois de uma batalha, o Pepe está exatamente onde estava e o NPC derrotado/poupado já reflete o resultado.

#### CORE-003 — Sistema de transições plugável `[ ]`
P1 · M · CORE-002
- **O quê:** a interface `ITransition` com implementações `FadeBlack(frames)`, `FadeWhite(frames)` (usada na confirmação do nome), `Cut`, `EncounterFlash` (ENC-002) e `SoulBreak` (BND-010). O `SceneManager` deixa de ter o fade preto embutido.

#### CORE-004 — `GameState` global serializável `[x]`
P0 · M · —
- **O quê:** separar de `PlayerData` (em `GameContext.hpp`) uma estrutura `GameState { PlayerStats, Inventory, Equipment, Flags (map<string,int>), RoomId, Position, PlayTime, KillsByArea, FurryState }`. Tudo que vai para o save mora aqui.
- **Aceite:** `GameState` faz roundtrip JSON → struct → JSON sem perda (teste em QA-010).

#### CORE-005 — Tela cheia com F4 e escala inteira opcional `[x]`
P1 · P · —
- **O quê:** F4 alterna tela cheia (como no Undertale). Opção "escala inteira" (letterbox em múltiplos de 640×480) para pixels sempre quadrados; opção de borda decorativa (P3, como as bordas do Undertale no PS4/Switch).

#### CORE-006 — "Segure ESC para sair" `[x]`
P1 · P · BUG-14
- **O quê:** segurar ESC por ~1 s mostra "QUITTING..." no canto superior esquerdo com os pontos progredindo; soltar cancela; completar fecha o jogo. ESC deixa de voltar ao título.

#### CORE-007 — Configuração do usuário `[ ]`
P1 · P · —
- **O quê:** expandir `config/user.json` com `lang`, `musicVolume`, `sfxVolume`, `fullscreen`, `integerScale`, `keybinds`, `border` e salvar em `%LOCALAPPDATA%/nUndertale/config.json` (não na pasta do jogo).

#### CORE-008 — Remapeamento de teclas e gamepad `[ ]`
P2 · M · CORE-007
- **O quê:** `InputService` com ações abstratas (`Confirm`, `Cancel`, `Menu`, direções) mapeadas para teclas e gamepad (raylib `IsGamepadButtonPressed`). Padrões iguais ao Undertale: Z/Enter, X/Shift, C/Ctrl. Rever o uso de **Space** como confirmação (o Undertale não usa) e de **X como backspace** na tela de nome.

#### CORE-009 — Buffer de input por quadro `[ ]`
P1 · P · CORE-001
- **O quê:** com a lógica a 30 Hz, um `IsKeyPressed` lido entre ticks pode se perder. Acumular os "pressed" entre ticks e consumir no próximo update.

#### CORE-010 — Sistema de eventos/flags `[x]`
P0 · M · CORE-004
- **O quê:** `Flags::get/set/inc` com chaves string (`"ruins.barky.spared"`, `"ruins.candy.taken"`), usado por diálogos condicionais, portas, NPCs e rotas.

#### CORE-011 — Cutscenes por script de passos `[ ]`
P1 · G · CORE-010, TXT-001
- **O quê:** um executor de sequências (`Cutscene { steps: [MoveNpc, Wait, Dialog, PlaySound, SetFlag, CameraPan, FadeOut, StartBattle] }`) carregado de JSON. Bloqueia o input do jogador enquanto roda.
- **Aceite:** a entrada do Barky (pulo dramático + fala, ROTEIRO Cena 3) é feita só com dados.

#### CORE-012 — Relógio de tempo de jogo `[x]`
P1 · P · CORE-004
- **O quê:** contar o tempo jogado (exibido no save e no continue como `0:00`).

#### CORE-020 — Avaliar Lua para padrões e cutscenes `[ ]`
P3 · G · D-09

---

### GFX — Renderização, fontes e estilo visual

#### GFX-001 — Paleta oficial em `Constants.hpp` `[x]`
P0 · P · —
- **O quê:** corrigir/adicionar: `UI_ORANGE = #FF7F27` (hoje `{255,165,0}`), `UI_YELLOW = #FFFF00`, `SOUL_RED = #FF0000`, `HP_RED = #FF0000`, `HP_YELLOW = #FFFF00`, `ENEMY_HP_GREEN = #00FF00`, `GRID_GREEN = #27AF4B`, `MISS_GRAY = #C0C0C0` (confirmar), `SOUL_BLUE = #0026FF`, `SOUL_GREEN = #00C000`, `SOUL_YELLOW = #FFFF00`, `SOUL_PURPLE = #D535D9` (confirmar), `KR_PURPLE = #FF00FF`.

#### GFX-002 — Pipeline de sprites e spritesheets `[ ]`
P0 · M · —
- **O quê:** o `SpriteSheet` carregado de PNG + JSON (formato Aseprite: frames, tags, duração por frame). O `AnimatedSprite` toca as tags. PNG com alfa substitui BMP + magenta (`pepe_char.bmp` atual).
- **Regra:** desenhar sempre com escala inteira (2× por padrão) e posição arredondada para inteiro.
- **Aceite:** Pepe, a alma, as balas e o Barky saem de spritesheets; nenhuma primitiva desenha personagem.

#### GFX-003 — Fontes em tamanho nativo e escala inteira `[ ]`
P0 · M · BUG-15
- **O quê:** carregar cada fonte no tamanho de pixel nativo (ex.: DTM Mono 16 px), renderizar a 2× (32 px) sem filtro. Melhor ainda: bitmap fonts (`.fnt` BMFont ou atlas PNG) para HUD, dano e botões. A API `ResourceService::drawText` recebe `FontId` + escala inteira, nunca tamanho arbitrário.
- **Aceite:** o texto "* Barky blocked your path!" tem glifos com pixels todos do mesmo tamanho (2×2), igual ao print de referência.

#### GFX-004 — Trocar a fonte principal pela Determination Mono `[ ]`
P0 · P · GFX-003, D-04
- **O quê:** adicionar `assets/fonts/DeterminationMono.ttf` (verificar licença), registrar como `FontId::Main`. `8bitoperator` fica como fallback.

#### GFX-005 — Fontes de personagem `[ ]`
P1 · P · GFX-003
- **O quê:** `FontId::Sans` (estilo comic, minúsculas) para o Senpai Sans e `FontId::Papyrus` (MAIÚSCULAS) para o Papyrus Streamer, escolhidas automaticamente pela "voz" do diálogo (TXT-006).

#### GFX-006 — Screen shake `[ ]`
P1 · P · —
- **O quê:** `Camera::shake(intensity, frames)` aplicado ao canvas virtual. Usado em: dano grande na alma, explosões, quedas, pulo do Barky e o ataque do chefe.

#### GFX-007 — Flash e tint de tela `[ ]`
P2 · P · —
- **O quê:** overlay branco/vermelho por N quadros (flash no encontro, flash do "SKYRIM" no nome, ataques especiais).

#### GFX-008 — Efeito de dissolução ("poeira") `[ ]`
P0 · M · GFX-002
- **O quê:** shader ou CPU: o sprite do inimigo vira poeira, linha por linha de cima para baixo, com partículas cinza subindo/espalhando. Usado na morte do inimigo (BND-003).

#### GFX-009 — Texto tremido (shaky/wavy) `[ ]`
P2 · P · TXT-001
- **O quê:** modo em que cada glifo treme ±1 px aleatoriamente por quadro (tela de nome, falas assustadas). Já existe algo parecido em `NameSelectScene`; extrair para o renderer de texto.

#### GFX-010 — Câmera 2D do overworld `[ ]`
P0 · M · OVW-003
- **O quê:** a câmera segue o jogador centralizada e travada nos limites da sala; suporta pan scriptado (cutscenes) e salas menores que a tela (centralizadas com bordas pretas).

---

### TXT — Sistema de texto e diálogo

#### TXT-001 — Motor de texto com markup `[ ]`
P0 · G · GFX-003
- **O quê:** o parser transforma uma string em tokens (caractere, pausa, cor, voz, rosto, velocidade, quebra, espera de input, fim). Sintaxe sugerida, próxima da do Undertale para facilitar a transcrição:
  - `&` quebra de linha · `/` espera Z e abre nova página · `%` fim
  - `^1`..`^9` pausa de N×(≈ 3 quadros)
  - `\R` `\Y` `\B` `\W` `\X` cores (vermelho, amarelo, azul, branco, reset)
  - `\E0`..`\E9` muda a expressão do retrato · `\V[name]` muda a voz · `\S[n]` velocidade
  - `{shake}`/`{/shake}` texto tremido
- **Aceite:** o renderer usado no overworld, na batalha, nas bolhas e no save é o mesmo.

#### TXT-002 — Quebra de linha automática com recuo deslocado `[ ]`
P0 · M · TXT-001
- **O quê:** quebrar por largura em pixels da área útil; quando a linha começa com "* ", as linhas seguintes recuam até a coluna depois de "* " (ver o print: "pick on you!" alinhado com "Loox").
- **Aceite:** nenhum texto passa da borda da caixa; o recuo bate com o print de referência.

#### TXT-003 — Velocidade de digitação e pausas automáticas `[ ]`
P0 · P · TXT-001, CORE-001
- **O quê:** padrão de 1 caractere por quadro (30 cps) (confirmar), com pausas automáticas extras depois de `.`, `!`, `?` e `,` (configurável por voz). Os sons de voz tocam em cada caractere não-espaço (algumas vozes, a cada 2).

#### TXT-004 — Controles do texto iguais ao Undertale `[x]`
P0 · P · BUG-11
- **O quê:** **X** (cancel) completa a página atual na hora; **Z** só avança quando a página terminou. Sem indicador piscando (o Undertale não mostra triângulo; remover o triângulo amarelo do `DialogBox`).

#### TXT-005 — Caixa de diálogo do overworld (posição e retrato) `[ ]`
P0 · M · TXT-001
- **O quê:** a caixa fica embaixo por padrão e **em cima quando o jogador está na metade de baixo da tela**. Retrato opcional à esquerda (sprite 2×, ex.: Furiell, Senpai Sans) com o texto deslocado à direita. Medidas a tirar de um print de overworld do Undertale (≈ x 32→608, y 320→470 embaixo; y 10→160 em cima) (confirmar).
- **Remover:** a linha "* SPEAKER:" amarela (o Undertale não mostra o nome de quem fala; quem fala é indicado pela voz/retrato/fonte).

#### TXT-006 — Vozes por personagem `[ ]`
P0 · M · TXT-001, AUD-002
- **O quê:** o registro `voices.json`: `{ "narrator": "snd_txt1", "furiell": "...", "sans": "...", "barky": "..." }` com pitch/variação opcional. Narrador (`* ...`) usa o "bip" padrão.

#### TXT-007 — Escolhas no diálogo `[ ]`
P0 · M · TXT-001
- **O quê:** token `{choice:Yes|No}` mostra as opções com a alma vermelha como cursor (esquerda/direita); o resultado vai para uma flag/callback. Usado em: save point, NPCs, Furiell ("Do you want to... ?"), lojas.

#### TXT-008 — Bolha de fala do inimigo `[ ]`
P0 · M · TXT-001
- **O quê:** um sprite de bolha branca com rabicho (9-slice), texto preto em fonte pequena, posicionada à direita do inimigo que fala, tamanho automático pelo texto, digitação com a voz do inimigo, Z avança e fecha.
- **Onde:** substitui o retângulo desenhado em `BattleSystem::render`.

#### TXT-009 — Texto "Check" no padrão do Undertale `[ ]`
P1 · P · ACT-001
- **O quê:** formato `* BARKY - ATK 4 DEF 2&* Description...`, com o nome em maiúsculas e ATK/DEF lidos dos dados.

#### TXT-010 — Todo texto vindo de arquivos `[ ]`
P0 · M · D-03, REPO-007
- **O quê:** zero strings de jogo no C++. Chaves em `data/dialogue/en/*.json` (`"ruins.sign.notice": "..."`). Reaproveitar e limpar `assets/lang/en.json` (herdado do Python).

---

### BUI — Batalha: layout e UI

#### BUI-001 — Caixa de batalha nas medidas oficiais `[x]`
P0 · P · GFX-001
- **O quê:** `m_arenaRect` padrão = borda externa (32,250)–(606,389), borda de 5 px branca, interior preto. Representar a caixa por **borda interna** (left/top/right/bottom) para animar o resize (BHL-001).
- **Onde:** `include/combat/BattleSystem.hpp` (`m_arenaRect{220,240,200,130}` → novo `BattleBox`).
- **Aceite:** sobreposição com o print de referência (50% de opacidade) bate pixel a pixel.

#### BUI-002 — Fundo de grade verde `[x]`
P0 · P · GFX-001
- **O quê:** linhas de 1 px em #27AF4B: horizontais em y = 9, 126, 244 (de x 15 a 623) e verticais em x = 15, 115, 218, 317, 420, 523, 623 (de y 9 a 244). Desenhar antes dos inimigos. Configurável por encontro (`background: "grid" | "none" | "<sprite>"`): chefes usam fundo próprio ou preto.

#### BUI-003 — Posicionamento de 1 a 3 inimigos `[x]`
P0 · P · BFL-004
- **O quê:** inimigos alinhados pelos **pés** em y ≈ 241; 1 inimigo = centro (x 320); 2 = ~x 213 e 427; 3 = ~x 110, 320, 530 (medido no print: Loox, Vegetoid, Migosp). Offset individual nos dados do inimigo.

#### BUI-004 — Remover o HP permanente do inimigo `[x]`
P0 · P · —
- **O quê:** apagar o "Barky HP: 30 / 30" fixo. O HP só aparece (a) na lista de alvos do FIGHT (barra verde/vermelha ao lado do nome) e (b) depois de um golpe (FGT-005).

#### BUI-005 — HUD 1:1 `[x]`
P0 · M · GFX-003
- **O quê:** desenhar cada elemento na posição medida (seção 2.1): nome em x=30, "LV n" em x=132, label "HP" (sprite pequeno) em x=244, barra em x=275 com largura `HPmax×1,2` e altura 20, números `hp / max` depois da barra. Nome com no máximo 6 caracteres (como no Undertale).
- **Furry meter:** ver FUR-003 (vai entrar como o "KR" do Sans, não como texto solto no meio do HUD).
- **Aceite:** com nome "CHARA", LV 2 e HP 16/24, sobrepondo o print de referência, os elementos coincidem (± fonte).

#### BUI-006 — Botões em sprite com ícones `[x]`
P0 · M · ART-020
- **O quê:** 4 sprites (FIGHT espada, ACT "boca"/coração, ITEM bolsa, MERCY X) em 2 estados (laranja/amarelo), 110×42, em x = 32, 185, 345, 500, y = 432. No estado selecionado, o ícone some e a alma vermelha é desenhada no lugar dele.
- **Onde:** `BattleSystem::render` (hoje `DrawRectangleLinesEx` + `DrawCircle` verde).
- **Paródia opcional:** ícones temáticos (FIGHT = mouse USB, ITEM = lata de Dew) **mantendo silhueta e posição** — decidir em ART-020.

#### BUI-007 — Alma em sprite `[x]`
P0 · P · D-01, ART-021
- **O quê:** sprite 16×16 (nativo 8×8 a 2× ou 16×16 nativo, confirmar) vermelho; variantes de cor por modo; frames de "rachada" e "estilhaços" para o game over.

#### BUI-008 — Menus dentro da caixa com a alma como cursor `[x]`
P0 · M · TXT-001
- **O quê:** todas as listas (alvos, ACTs, itens, mercy) desenhadas **dentro da caixa grande**, em grade de 2 colunas, com a alma vermelha à esquerda da opção selecionada (no Undertale não há destaque amarelo no texto do ACT; o cursor é só a alma). Posições: coluna 1 com o "*" em x ≈ 100, coluna 2 em x ≈ 340; linhas em y ≈ 270 e 302 (confirmar).
- **Remover:** o padrão atual de texto amarelo + "* " só no selecionado.

#### BUI-009 — Nomes de inimigos poupáveis em amarelo `[x]`
P0 · P · BUI-008
- **O quê:** na lista de alvos (FIGHT/ACT/MERCY), o nome do inimigo poupável aparece em amarelo; em MERCY, "* Spare" fica amarelo se **algum** inimigo puder ser poupado.

#### BUI-010 — Animação idle dos inimigos `[ ]`
P1 · M · GFX-002
- **O quê:** cada inimigo tem um idle (balanço, respiração, partes que se mexem separadas). Ex.: Barky balança o rabo e mexe as orelhas; o idle para quando ele toma dano e quando é poupado.

#### BUI-011 — Inimigo poupado/fugido fica translúcido `[ ]`
P1 · P · BND-002
- **O quê:** depois de poupado, o sprite fica ~50% transparente e para de animar até a batalha acabar (em lutas com vários inimigos, os outros continuam).

---

### BFL — Batalha: fluxo e máquina de estados

#### BFL-001 — Reescrever a máquina de estados da batalha `[ ]`
P0 · G · BUG-02, BUI-008
- **O quê:** estados explícitos, na ordem real do Undertale:
  1. `Intro` — texto de encontro ("* Barky blocked your path!" / "* X and co. decided to pick on you!"), botões já ativos.
  2. `ChooseButton` — esquerda/direita entre os 4 botões; a alma fica no botão.
  3. `ChooseTarget` — lista de inimigos (FIGHT mostra barras de HP; ACT e MERCY não).
  4. `ChooseAct` / `ChooseItem` / `ChooseMercy`.
  5. `FightBar` (FGT) → `FightAnim` (corte + número de dano).
  6. `ActionText` — texto do resultado (ACT/ITEM/MERCY); Z avança.
  7. `EnemyDialogue` — bolhas (TXT-008), uma por inimigo que fala.
  8. `BoxToArena` — anima a caixa até o tamanho do ataque, a alma aparece no centro.
  9. `EnemyAttack` — o padrão roda pelo tempo definido.
  10. `ArenaToBox` — a caixa volta, a alma some.
  11. `FlavorText` — texto de "clima" do turno → volta para 2.
  12. `Victory` / `Spared` / `Fled` / `GameOver`.
- **X** volta um nível em qualquer submenu.
- **Aceite:** diagrama do fluxo em `docs/battle_flow.md`; nenhum estado pula etapas; o BUG-02 deixa de ocorrer.

#### BFL-002 — Textos de "flavor" por turno `[ ]`
P0 · M · BFL-001, ACT-001
- **O quê:** cada inimigo tem pools de flavor text: `default`, `lowHp` (HP < 30%), `spareable`, condicionais por flag (ex.: depois do "Cringe Check": "* Barky is still blushing."). Escolha aleatória sem repetir a última.

#### BFL-003 — Diálogos do inimigo por gatilho `[ ]`
P0 · M · BFL-001
- **O quê:** a bolha do inimigo depende do que o jogador fez no turno (resposta ao ACT específico, ao dano, ao item usado) com fallback aleatório. Ex.: ROTEIRO Cena 3: "H-hey! Stop looking at the zipper!..." depois do "Cringe Check".

#### BFL-004 — Encontros com vários inimigos `[ ]`
P0 · G · BFL-001
- **O quê:** `Encounter { enemies[1..3], introText, background, music, canFlee }`. Os ataques de todos os inimigos vivos/não-poupados se somam no mesmo turno (cada um com seu padrão); a vitória vem quando todos estão mortos, poupados ou fugiram.

#### BFL-005 — Dados de encontro e inimigo em JSON `[ ]`
P0 · M · REPO-007, BFL-004
- **O quê:** `data/enemies/barky.json` com `name, hp, atk, def, exp, gold, sprite, voice, check, acts[], flavor{}, dialogue{}, patterns[], spareCondition, fleeable` (schema completo no Apêndice A). `data/encounters/*.json` referencia os inimigos.
- **Onde:** substitui o construtor hardcoded em `BattleScene::BattleScene`.

#### BFL-006 — Condições de spare declarativas `[ ]`
P0 · M · BFL-005
- **O quê:** `mercy` como contador (0–100) ou conjunto de flags: `spareWhen: { all: ["act.cringe_check", "act.offer_dew"] }` ou `{ mercy: 100 }`. Também valem "HP abaixo de X%" e "turnos sobrevividos ≥ N".

#### BFL-007 — Música de batalha por encontro `[ ]`
P1 · P · AUD-001
- **O quê:** inimigos comuns usam o tema de batalha padrão; chefes usam o próprio tema; a música do overworld **para** na transição (ENC-002) e volta ao sair.

---

### FGT — Batalha: FIGHT

#### FGT-001 — Alvo elíptico do ataque em sprite `[ ]`
P0 · M · ART-022
- **O quê:** um sprite "target" (elipse com linhas verticais e faixas, estilo Undertale) do tamanho do interior da caixa (565×130). Substitui os retângulos cinza/amarelo atuais.

#### FGT-002 — Barra de ataque `[ ]`
P0 · M · FGT-001
- **O quê:** uma barra vertical branca com contorno preto entra por um lado **aleatório** (BUG-17) e atravessa a caixa em ~1,5–2 s (confirmar); Z para a barra. Depois do golpe, a barra pisca branco/preto alternando por alguns quadros. Se a barra sair do outro lado sem Z: "MISS".
- **Armas especiais** (P2): várias barras (estilo Real Knife/Burnt Pan); dados da arma definem `bars`, `speed`.

#### FGT-003 — Fórmula de dano dos dados `[ ]`
P0 · P · PRG-001
- **O quê:** aplicar a fórmula da seção 2.2 com AT do jogador + AT da arma − DF do inimigo. Valor mínimo 1 (ou "MISS" se a barra não foi parada).
- **Aceite:** teste unitário com casos de borda (centro exato, borda, fora) (QA-011).

#### FGT-004 — Animação de corte no inimigo `[ ]`
P0 · M · ART-023
- **O quê:** um corte de ~6 frames sobre o inimigo (paródia ROTEIRO: cursor de mouse/"hitmarker" de FPS, mantendo a duração e a posição do corte original), com som de corte e, em seguida, som de dano.

#### FGT-005 — Número de dano e barra de HP do inimigo `[ ]`
P0 · M · GFX-003
- **O quê:** o número de dano (fonte de dano própria, vermelho) aparece acima do inimigo, quica para cima e cai/assenta; embaixo dele aparece a barra de HP (fundo vermelho/cinza, verde #00FF00) que **diminui animada**; some depois de ~1 s. "MISS" em cinza sem barra.

#### FGT-006 — Inimigo treme ao tomar dano `[ ]`
P0 · P · —
- **O quê:** o sprite oscila horizontalmente com amplitude decrescente (ex.: ±8, −6, +4, −2, 0 px em ~10 quadros).

#### FGT-007 — Morte por golpe → vaporização `[ ]`
P0 · P · GFX-008, BND-003

#### FGT-008 — Hitmarker paródico `[ ]`
P2 · P · FGT-004
- **O quê:** o ROTEIRO pede "som de hitmarker de FPS" no crítico. Tocar o hitmarker **no lugar** do som de corte quando `|distância| ≤ 12`, mantendo o resto da sequência idêntica.

---

### ACT — Batalha: ACT

#### ACT-001 — ACTs definidos por inimigo nos dados `[ ]`
P0 · M · BFL-005
- **O quê:** `acts: [{ id, label, text, effects: { mercy:+N, setFlag, spareable, heal, furry:-N }, requires?, responseDialogue? }]`. "Check" é sempre o primeiro e automático.
- **Aceite:** adicionar um ACT novo ao Barky não exige recompilar.

#### ACT-002 — ACTs do Barky conforme o ROTEIRO `[ ]`
P0 · P · ACT-001, NAR-002
- **O quê:** `Check`, `Cringe Check` (aponta o zíper → resposta "b-baka!"), `Offer Mountain Dew` (deixa poupável → "citrus blast flavor?!"), `Debate Anime Lore` (confunde, pula o ataque dele?), `Laser Pointer` (ROTEIRO: "fica desorientado com laser vermelho"). Definir quais combinações deixam o nome amarelo (`D-06`/NAR-002).

#### ACT-003 — ACTs que mudam o ataque inimigo `[ ]`
P1 · M · ACT-001, BHL-004
- **O quê:** efeitos como "o próximo ataque fica mais fácil/rápido/curto" ou "o inimigo pula o turno" (estilo Vegetoid "Dinner" = balas verdes que curam).

#### ACT-004 — ACTs em grupo e com acompanhante `[ ]`
P3 · M · BFL-004
- **O quê:** ACTs que afetam todos ou exigem dois inimigos específicos (estilo Dogamy/Dogaressa).

---

### ITM — Batalha: ITEM

#### ITM-001 — Base de itens dirigida por dados `[ ]`
P0 · M · REPO-007
- **O quê:** `data/items.json`: `{ id, name, shortName (≤ 9 caracteres, nome de batalha), type: consumable|weapon|armor|key, heal, at, df, furry, speedBuff, info, useText, sellPrice, buyPrice }`.
- **Itens do ROTEIRO:** `Mountain Dew (Warm)` (+10 HP e velocidade temporária), `Stale Doritos` (+6 HP), `Duct Tape` (−15% furry), arma inicial `Frayed USB Optical Mouse` (≈ Stick), armadura inicial `One-Sided Broken Gaming Headset` (≈ Bandage).

#### ITM-002 — Menu de itens na batalha `[ ]`
P0 · M · ITM-001, BUI-008
- **O quê:** 2 colunas × 2 linhas por página, "PAGE 1"/"PAGE 2" no canto inferior direito da caixa, esquerda/direita troca de coluna e de página, Z usa, X volta.

#### ITM-003 — Textos de uso no padrão Undertale `[ ]`
P0 · P · ITM-001
- **O quê:** "* You drank the Mountain Dew.&* You recovered 10 HP!" / "* Your HP was maxed out." com o som de cura. Usar um item **gasta o turno** (o inimigo ataca em seguida).

#### ITM-004 — Inventário de 8 slots `[ ]`
P0 · P · CORE-004
- **O quê:** limite de 8 itens como no Undertale; o item novo com o inventário cheio dá "* You're carrying too much." (ou similar).

#### ITM-005 — Efeito de velocidade do Mountain Dew `[ ]`
P2 · P · ITM-001, BHL-006
- **O quê:** buff de velocidade da alma por N turnos (paródia direta do ROTEIRO), com indicação visual sutil (rastro).

---

### MRC — Batalha: MERCY / Fuga

#### MRC-001 — Spare `[x]`
P0 · P · BFL-006
- **O quê:** só funciona se o alvo é poupável; senão, "* You spared Barky.&* But Barky wasn't yellow..." (paródia) e o turno passa. Inimigo poupado → poeira/nuvem + translucidez (BUI-011) + som de spare.

#### MRC-002 — Flee `[ ]`
P0 · M · ART-024
- **O quê:** chance de fuga (base ~50%, maior com mais turnos, confirmar); sucesso: a alma vira o sprite "correndo" (com perninhas) e sai pela esquerda, com texto aleatório ("* I'm outta here." / paródia: "* Pepe alt-F4'd out of the fight."); falha: o turno passa. Chefes: `canFlee=false`.
- **ROTEIRO:** "Pepe foge de costas com nuvenzinhas de poeira" → usar as nuvens de poeira na animação de fuga da alma.

#### MRC-003 — Recompensa de spare/fuga `[ ]`
P0 · P · PRG-001
- **O quê:** spare: 0 EXP e gold do inimigo. Fuga: "* Escaped..." sem recompensa (no Undertale, fugir depois de dar dano dá EXP/gold parciais, confirmar).

---

### BHL — Batalha: turno inimigo e bullet hell

#### BHL-001 — Redimensionamento animado da caixa `[x]`
P0 · M · BUI-001
- **O quê:** `BattleBox::resizeTo(rect, frames)` interpolando as 4 bordas. Cada padrão de ataque declara o tamanho da caixa (padrão ~165×140 centralizado, base em y 389) (confirmar). A alma aparece no centro quando o resize termina.
- **Aceite:** a transição texto → ataque → texto fica igual à de uma gravação de referência quadro a quadro.

#### BHL-002 — Recorte das balas no interior da caixa `[x]`
P0 · P · BUG-10
- **O quê:** `BeginScissorMode` no interior da caixa ao desenhar as balas (padrões que precisam sair da caixa marcam `clip:false`). Balas fora dos limites + margem são destruídas.

#### BHL-003 — Hitboxes precisas `[x]`
P0 · M · GFX-002
- **O quê:** a alma tem hitbox pequena central (confirmar ~8×8); balas com hitbox por tipo (círculo, retângulo rotacionado, máscara por pixel opcional para formas irregulares). Debug overlay (QA-002) desenha todas.

#### BHL-004 — Framework de padrões de ataque `[ ]`
P0 · G · D-09, BFL-005
- **O quê:** `IAttackPattern { boxSize(); duration(); onStart(); update(frame); }` + `PatternRegistry` com o nome do JSON → classe. Parâmetros (velocidade, densidade, dano) no JSON do inimigo. Seleção por turno: sequencial, aleatória, ou condicionada a flags/HP.

#### BHL-005 — Dano por bala e DF do jogador `[x]`
P0 · P · BUG-07, PRG-002
- **O quê:** cada bala tem `damage`; o dano final = `max(1, damage − floor(DF/5))` (confirmar). Som de dano, i-frames (BHL-007), +furry (FUR-002).

#### BHL-006 — Movimento da alma fiel `[x]`
P0 · P · CORE-001
- **O quê:** 4 px/quadro nas 8 direções (sem normalizar a diagonal, como no Undertale) (confirmar); segurar X reduz a velocidade pela metade (confirmar se é Undertale ou só Deltarune; se não for, deixar de fora). Limitado ao interior da caixa respeitando o tamanho do sprite.

#### BHL-007 — I-frames e feedback de dano `[x]`
P0 · P · BUI-007
- **O quê:** depois do dano, a alma pisca (alterna visível/invisível a cada 2 quadros) por ~30 quadros; som "hurt"; screen shake leve (GFX-006). Armaduras podem aumentar a duração.

#### BHL-008 — Balas brancas/azuis/laranjas `[ ]`
P1 · M · BHL-004
- **O quê:** além das brancas normais: **azuis** só causam dano se a alma estiver se movendo; **laranjas** só se estiver parada. Essencial para a luta do Papyrus/Sans.

#### BHL-009 — Balas verdes de cura `[ ]`
P2 · P · BHL-004
- **O quê:** a bala que cura ao tocar (estilo Vegetoid). Paródia: latas de Mountain Dew verdes que curam (o ROTEIRO já tem latas quicando como bala).

#### BHL-010 — Modo de alma azul (gravidade) `[ ]`
P1 · G · D-01, BHL-004
- **O quê:** gravidade, pulo com seta para cima (altura proporcional ao tempo segurado), plataformas, "slam" que joga a alma contra a parede. Necessário para Papyrus Streamer e Senpai Sans.

#### BHL-011 — Modo de alma verde (escudo) `[ ]`
P2 · G · BHL-004
- **O quê:** a alma fica presa no centro e um escudo gira nas 4 direções para bloquear flechas. Para a paródia da Undyne (C3+).

#### BHL-012 — Modo de alma amarela (atirador) `[ ]`
P3 · G · BHL-004
- **O quê:** a alma vira para cima e atira com Z. Para a paródia do Mettaton (C3+).

#### BHL-013 — Modo de alma roxa (fios) `[ ]`
P3 · G · BHL-004
- **O quê:** a alma presa em 3 linhas horizontais; cima/baixo troca de linha. Para a paródia da Muffet (C3+).

#### BHL-014 — Padrões do Barky `[ ]`
P0 · M · BHL-004, ART-012
- **O quê:** 3–4 padrões:
  1. **UwU Rain** — letras "UWU" (sprite, não `DrawText`) caindo em colunas com leve zigue-zague.
  2. **Dew Bounce** — latas que quicam nas paredes da caixa com gravidade.
  3. **Awoo Howl** — ondas sonoras (arcos) vindas dos lados com uma abertura para passar.
  4. **Tail Swipe** (quando HP < 50%) — um rabo varre a caixa horizontalmente; pular por cima com timing (sem modo azul, só desviando para cima/baixo).
- **Aceite:** dificuldade de "primeiro chefe do Undertale": dá para passar sem tomar dano depois de 2–3 tentativas.

#### BHL-015 — Texto "pense" sobre a alma `[ ]`
P3 · P · —
- **O quê:** easter egg (ex.: balão minúsculo "ribbit" quando a alma fica parada muito tempo).

---

### BND — Batalha: finais (vitória, poeira, level up, game over)

#### BND-001 — Tela de vitória `[ ]`
P0 · P · PRG-001
- **O quê:** "* YOU WON!&* You earned X EXP and Y gold." na caixa grande, com a música cortada. Z → volta ao overworld com fade.

#### BND-002 — Sequência de spare `[ ]`
P0 · P · MRC-001
- **O quê:** som de spare + nuvenzinhas saindo do inimigo + translucidez; se era o último inimigo, vai para BND-001 com 0 EXP.

#### BND-003 — Vaporização `[ ]`
P0 · P · GFX-008
- **O quê:** o inimigo morto vira poeira (GFX-008) com o som de "vaporized"; o gold/EXP é contado no fim.

#### BND-004 — Level up `[ ]`
P0 · P · PRG-001
- **O quê:** quando a EXP passa do limite: a linha extra "* Your LOVE increased." (paródia: "* Your VITRIOL increased.") + som de level up; HPmax/AT/DF atualizados.

#### BND-010 — Game over 1:1 + furrificação `[x]`
P0 · G · CORE-003, FUR-004
- **O quê:** sequência do Undertale com a reviravolta do ROTEIRO:
  1. Tudo congela; a tela fica preta, menos a alma, que fica onde estava.
  2. ~0,5 s depois a alma **racha** (som "break1"); ~1 s depois **se parte em 4–6 estilhaços** que voam com gravidade (som "break2"). Paródia: os estilhaços viram **tufos de pelo**.
  3. O logo "GAME OVER" aparece num fade lento (paródia: "GAME OVER" com orelhinhas de gato no "O", ou "FURRY OVER" — decidir em NAR-011).
  4. Música de game over (original, no clima de "Determination").
  5. Texto digitado com voz grave (no Undertale é o Asgore). Paródia sugerida: a voz do **líder da raid do Discord**: "You cannot ragequit just yet... / PEPE! / Stay determined..." (usa o nome escolhido).
  6. Z → fade para o último save, com HP cheio.
- **Aceite:** gravando e comparando com o game over do Undertale, os tempos de cada etapa batem em ±3 quadros.

#### BND-011 — Variantes de game over por estágio furry `[ ]`
P2 · M · BND-010, FUR-001
- **O quê:** a cada game over, uma frase diferente que reconhece a furrificação progressiva ("You feel your ears getting fluffier...").

---

### ENC — Encontros e transição para batalha

#### ENC-001 — "!" de alerta `[ ]`
P1 · P · ART-030
- **O quê:** nos encontros scriptados, um balão "!" aparece sobre a cabeça do Pepe por ~15 quadros antes da transição (com som).

#### ENC-002 — Transição de encontro 1:1 `[ ]`
P0 · M · CORE-003, BUI-007
- **O quê:** sequência exata:
  1. A música do overworld para.
  2. A tela fica preta, menos a **alma vermelha** desenhada sobre o peito do Pepe.
  3. A alma pisca 3 vezes (visível/invisível), com um "bip" a cada piscada.
  4. A alma **desliza** até a posição do botão FIGHT (x ≈ 40, y ≈ 446) em ~15–20 quadros, com som de "queda".
  5. A batalha aparece (a alma já está no botão FIGHT).
- **Onde:** substitui o fade preto atual de `changeScene` em `OverworldScene` e o atalho B.
- **Aceite:** igual a uma gravação de referência; zero frames de fade cinza.

#### ENC-003 — Encontros aleatórios por passos `[ ]`
P1 · M · OVW-003
- **O quê:** cada sala define `encounterTable` + `stepsMin/stepsMax`; um contador de passos (distância andada) dispara o encontro ponderado. Sem encontro nos primeiros N passos depois de uma batalha ou de entrar na sala.

#### ENC-004 — Esgotamento de área (para a rota Toxic Gamer) `[ ]`
P2 · M · ENC-003, PRG-030
- **O quê:** contador de kills por área; ao atingir o limite (Ruínas = 20 no Undertale), os encontros param e aparece "* But nobody came." — paródia: "* But nobody logged in."

#### ENC-005 — Sair do atalho de debug B `[x]`
P0 · P · QA-001
- **O quê:** a tecla B só funciona com `NUNDERTALE_DEBUG`; vira item do menu de debug (QA-003).

---

### OVW — Overworld

#### OVW-001 — Remover o HUD do overworld `[x]`
P0 · P · MNU-001
- **O quê:** o Undertale não tem HUD em exploração. Apagar a barra inferior com NOME/LV/HP/G/FURRY e as dicas "[Z: Interact | B: Battle | ESC: Menu]". As informações passam para o menu C (MNU-001).

#### OVW-002 — Salas maiores que a tela + câmera `[ ]`
P0 · M · GFX-010, OVW-003
- **O quê:** a sala é um mapa de tamanho livre; a câmera segue o Pepe. Substitui o `test.bmp` 640×480 com os pilares desenhados por `DrawRectangle`.

#### OVW-003 — Loader de mapas Tiled `[ ]`
P0 · G · D-08
- **O quê:** ler o JSON do Tiled: tilesets, camadas de tile (com ordem de desenho), camada de colisão (retângulos **e polígonos**, para as paredes diagonais das Ruínas), objetos (`door`, `sign`, `npc`, `savepoint`, `trigger`, `spawn`, `encounterZone`) com propriedades customizadas.
- **Aceite:** a sala de teste atual é refeita no Tiled e funciona igual, sem nenhuma coordenada no C++.

#### OVW-004 — Ordenação por profundidade (Y-sort) `[ ]`
P0 · P · OVW-003
- **O quê:** Pepe, NPCs e objetos altos desenhados por Y da base; camada "above" para o que fica sempre por cima (arcos, topo de pilares).

#### OVW-005 — Colisão deslizante e paredes diagonais `[ ]`
P0 · M · OVW-003
- **O quê:** colisão por eixo (já existe) + deslizar ao longo das paredes diagonais (como andar na diagonal contra a parede em 45° no Undertale). Caixa de colisão do Pepe só nos pés (ajustar a atual `{x+5, y+38, 28, 18}` ao sprite novo).

#### OVW-006 — Movimento fiel do Pepe `[x]`
P0 · P · CORE-001
- **O quê:** 3 px de sala por quadro, que na view 2× dá 6 px de tela por quadro = 180 px/s a 30 fps (confirmar). **Sem normalizar a diagonal** (na diagonal o Frisk anda 3 px em cada eixo). A direção do sprite segue a regra do Undertale: na diagonal, mantém a direção que já estava apertada antes. Sem corrida (D-10).
- **Hoje:** `PLAYER_SPEED = 180 px/s` já bate em linha reta, mas a diagonal é normalizada e a escolha de direção usa o eixo dominante → trocar.

#### OVW-007 — Interação ao estilo Undertale `[ ]`
P0 · P · OVW-003
- **O quê:** Z interage com o objeto **na frente** do Pepe (caixa de interação curta na direção do olhar); NPCs viram para o jogador ao conversar e voltam à direção original depois.

#### OVW-008 — Portas e transições de sala `[ ]`
P0 · M · OVW-003, CORE-003
- **O quê:** objeto `door { targetRoom, targetSpawn, facing }`; ao tocar, fade preto rápido (~8 quadros, confirmar), troca de sala, o Pepe aparece no spawn olhando na direção certa. A música continua se for a mesma da área.

#### OVW-009 — NPCs com rotinas `[ ]`
P1 · M · OVW-003, CORE-011
- **O quê:** NPCs parados, andando em rota (waypoints), seguindo o jogador (Furiell guiando pelas Ruínas), com diálogos condicionados a flags e contagem de conversas ("talkCount").

#### OVW-010 — Persistência de estado da sala `[ ]`
P0 · M · CORE-010, BUG-04
- **O quê:** objetos pegos/usados, NPCs derrotados, puzzles resolvidos e portas abertas lidos das flags ao entrar na sala.

#### OVW-011 — Objetos interativos genéricos `[ ]`
P0 · M · OVW-003, TXT-001
- **O quê:** tipos de dados: `sign` (texto), `examine` (texto com variações por talkCount), `itemPickup` (dá um item uma vez), `switch` (muda flag), `container` (ex.: tigela de doces "Take one" → paródia "Take one Dorito"), `bed`, `mirror` ("* It's you!" → "* It's you, Pepe. Feels bad man.").

#### OVW-012 — Puzzles base `[ ]`
P1 · G · OVW-011
- **O quê:** blocos de construção reutilizáveis: interruptores no chão/parede, espinhos que abaixam com flag, pedras empurráveis em grid, piso que racha e derruba para a sala de baixo, alavancas com ordem, piso de gelo (Snowdinha).

#### OVW-013 — Sons de passos `[ ]`
P3 · P · AUD-002
- **O quê:** o Undertale **não** tem som de passo no chão comum; só em superfícies especiais (folhas secas crocantes, neve, água). Implementar por tile.

#### OVW-014 — Pilhas de folhas e tiles especiais `[ ]`
P2 · P · OVW-003
- **O quê:** tiles que tocam som ao pisar (folhas das Ruínas → paródia: embalagens de salgadinho amassadas, como diz o ROTEIRO Cena 1).

---

### MNU — Menu do overworld (ITEM / STAT / CELL)

#### MNU-001 — Menu C `[x]`
P0 · M · TXT-001
- **O quê:** C/Ctrl abre: uma caixa pequena no canto superior esquerdo com NOME, "LV 1", "HP 20/20", "G 10" e, embaixo, outra caixa com `ITEM`, `STAT`, `CELL` e a alma como cursor. Quando o Pepe está na metade de cima da tela, as caixas vão para baixo. Medidas a tirar de referência (confirmar).
- **Aceite:** abrir/fechar com C e X; o jogo fica pausado enquanto aberto.

#### MNU-002 — ITEM `[x]`
P0 · M · MNU-001, ITM-001
- **O quê:** a lista de até 8 itens numa caixa grande; escolher um item abre `USE / INFO / DROP`. INFO mostra a descrição ("* \"Mountain Dew (Warm)\" - Heals 10 HP&* Tastes like regret and citrus."). DROP: "* The Duct Tape was thrown away." Equipar arma/armadura troca com a atual.

#### MNU-003 — STAT `[x]`
P0 · M · MNU-001, PRG-001
- **O quê:** uma caixa grande no formato do Undertale:
  ```
  "PEPE"
  LV  1
  HP  20 / 20

  AT  0 (0)     EXP: 0
  DF  0 (0)     NEXT: 10

  WEAPON: Frayed USB Mouse
  ARMOR:  Broken Headset
  GOLD: 10
  ```
  Paródia: linha extra `FUR: 0%` (FUR-003) e uma linha de "KILLS" que só aparece na rota Toxic Gamer.

#### MNU-004 — CELL (celular) `[ ]`
P1 · M · MNU-001, NAR-004
- **O quê:** contatos liberados por flag. No Capítulo 1: "Furiell" com as opções `Say Hello`, `Puzzle Help`, `About Yourself`, `Call Her "Mod"` (paródia do "Call Her Mom"), cada uma com diálogo por sala/estado. Paródia: o celular é um "Nokia com Discord".

#### MNU-005 — Caixa dimensional (armazenamento) `[ ]`
P3 · M · ITM-004
- **O quê:** a partir da Snowdinha: um baú compartilhado entre locais (paródia: "Cloud Storage — 3 GB grátis").

---

### SAV — Save point, save/load e meta-persistência

#### SAV-001 — Save point em sprite e animação `[ ]`
P0 · P · ART-031
- **O quê:** a estrela amarela de 4 pontas em sprite, com a animação de 2 frames do Undertale. Paródia do ROTEIRO: a estrela brilhando **em cima de uma garrafa de refrigerante vintage**. Hoje é feita com 3 `DrawRectangle`.

#### SAV-002 — Fluxo do save point `[x]`
P0 · M · TXT-007, SAV-003
- **O quê:**
  1. Texto de determinação da sala ("* The scent of lukewarm energy drinks and stale Doritos fills you with DETERMINATION.") + HP cheio + som de cura.
  2. A caixa de save centralizada: `NOME   LV 1   0:42` / `Ruins - Entrance` / `Save   Return`, com a alma de cursor.
  3. Save: as linhas viram **amarelo**, som de save, "File saved." no lugar das opções; Z fecha.
- **Texto por save point:** cada save tem o próprio texto de determinação (dados), que pode mudar com flags (rota Toxic Gamer: "* Determination.").

#### SAV-003 — Arquivo de save `[x]`
P0 · M · CORE-004
- **O quê:** `%LOCALAPPDATA%/nUndertale/file0.json` com o `GameState` completo + versão do schema; escrita atômica (arquivo temporário + rename). Um slot, como no Undertale.

#### SAV-004 — Continue/Reset `[x]`
P0 · M · SAV-003, FRT-005
- **O quê:** com save existente, o título mostra a tela de continue (FRT-005). Reset apaga o save **mas não a meta-persistência** (SAV-005).

#### SAV-005 — Meta-persistência (memória entre resets) `[ ]`
P2 · M · SAV-003, NAR-012
- **O quê:** `meta.json` separado (equivalente ao `undertale.ini`): nº de resets, se já matou alguém, se já zerou alguma rota, nº de furrificações completas (D-05). Usado por falas que "lembram" (o Senpai Sans quebrando a quarta parede, a paródia do Flowey).

#### SAV-006 — Recuperação de save corrompido `[x]`
P2 · P · SAV-003
- **O quê:** se o JSON falhar, tentar o `file0.bak`; se falhar também, avisar e começar do zero sem crashar.

---

### PRG — Progressão, equipamento, economia e rotas

#### PRG-001 — LV, EXP e atributos `[x]`
P0 · M · CORE-004
- **O quê:** `data/lv_table.json` com EXP acumulada por LV (seção 2.2), `HPmax = 16 + 4·LV`, AT e DF por LV (tabela da wiki, confirmar). EXP/gold vêm dos dados do inimigo.

#### PRG-002 — Equipamento `[ ]`
P0 · P · ITM-001
- **O quê:** arma (AT, barras de ataque, velocidade da barra) e armadura (DF, i-frames extras). Iniciais: Frayed USB Optical Mouse e One-Sided Broken Gaming Headset.

#### PRG-003 — Gold e lojas `[ ]`
P1 · G · TXT-007
- **O quê:** a tela de loja do Undertale: o retrato do vendedor em cima, o menu `Buy / Sell / Talk / Exit` embaixo à direita, a lista de itens com preço, a caixa de diálogo do vendedor. Primeira loja: Snowdinha (C2). Paródia na venda: "Steam Market" com taxa absurda.

#### PRG-004 — Vendas de aranha / lojas de NPC `[ ]`
P2 · M · PRG-003
- **O quê:** paródia da "Spider Bake Sale" das Ruínas → "NFT Bake Sale" (itens caros e inúteis, ex.: "Spider Donut" → "JPEG Donut").

#### PRG-020 — Flags de progressão do capítulo `[ ]`
P0 · P · CORE-010
- **O quê:** lista central de flags documentada em `data/flags.md` (ex.: `ruins.barky.state = none|spared|killed`, `ruins.furiell.state`, `ruins.exit.open`).

#### PRG-030 — Rastreamento de rota `[ ]`
P1 · M · PRG-020, D-07
- **O quê:** contadores de kill, de spare e de fuga; critérios das rotas Wholesome (0 kills), Normie (o resto) e Toxic Gamer (esgotar as áreas + matar os chefes). Lido por diálogos, finais e pelo Senpai Sans.

---

### FRT — Front-end (disclaimer, intro, título, nome, settings)

#### FRT-001 — Ordem de boot igual à do Undertale `[ ]`
P1 · P · SAV-004
- **O quê:** primeiro boot: Disclaimer (exclusivo do nUndertale, ok) → Intro → título "nUNDERTALE" → **tela de instruções com "Begin Game / Settings"** → nome → fade branco → sala da queda. Com save: Intro (se ficar parado) → título → Continue/Reset.
- **Hoje:** título com "New Game / Instructions / Exit" e as instruções numa tela à parte.

#### FRT-002 — Intro (lore) 1:1 `[ ]`
P1 · M · TXT-001
- **O quê:** imagens de 400×220 (as atuais têm 398×214 → ajustar o canvas da arte) em x = 120, com o texto digitado embaixo (fonte principal, 2×), um fade preto entre cartas, a música da intro, e Z acelera. **A última imagem é alta e rola verticalmente** (a queda) antes do título — hoje é uma animação de 187 BMPs (`assets/img/lore10/`); trocar por uma imagem alta + scroll, ou por uma spritesheet PNG.
- **Texto:** revisar as cartas (hoje a carta 2 diz "yiffing the GAMERS like true degenerates", que contradiz o ROTEIRO "Era das Grandes Flame Wars"). Ver NAR-001.

#### FRT-003 — Título 1:1 `[ ]`
P1 · P · GFX-004
- **O quê:** logo "nUNDERTALE" (fonte Monster Friend, branco) centralizado, sem flutuar (o Undertale não flutua; manter o efeito só se for decisão de estilo), "[PRESS Z OR ENTER]" pequeno e cinza que aparece depois de ~1 s, som de "intronoise" ao aparecer. As frases aleatórias (`RANDOM_QUOTES`) podem ficar como splash de paródia, em cinza, embaixo.
- **Ociosidade:** parado ~20 s no título → a intro roda de novo.

#### FRT-004 — Tela de instruções/menu inicial `[ ]`
P1 · P · FRT-001
- **O quê:** o layout do Undertale: `--- Instruction ---` em cinza, a lista `[Z or ENTER] - Confirm`, `[X or SHIFT] - Cancel`, `[C or CTRL] - Menu (In-game)`, `[F4] - Fullscreen`, `[Hold ESC] - Quit`, `When HP is 0, you become a furry.` (já existe, manter), e embaixo `Begin Game` / `Settings` com seleção amarela.

#### FRT-005 — Tela de Continue `[ ]`
P1 · M · SAV-004
- **O quê:** NOME, LV, tempo e local do save; `Continue` / `Reset`; `Settings` embaixo. **Fundo com a arte da última área** e música que muda com o progresso (como o menu do Undertale).

#### FRT-006 — Tela de nome 1:1 `[ ]`
P0 · M · GFX-009
- **O quê:** "Name the fallen gamer." (já existe); grade com as letras **maiúsculas A–Z (7 colunas) e minúsculas a–z (7 colunas)** como no Undertale; botões `Quit`, `Backspace`, `Done` embaixo; letras tremendo levemente; limite de 6 caracteres.
- **Paródia:** manter os botões especiais `[PRO]` e `[GAMER]` só se couberem sem quebrar o layout (sugestão: entram como easter egg de nome, não como botão).

#### FRT-007 — Confirmação de nome 1:1 `[ ]`
P0 · M · FRT-006, CORE-003
- **O quê:** "Is this name correct?" (paródia atual: "Are you sure about that?", ok), o nome **cresce devagar** de pequeno para grande no centro enquanto treme, `No` / `Yes` embaixo; Yes → **fade branco longo (~3 s)** com som crescente (cymbal/"swoosh") direto para a sala da queda. Nomes proibidos mostram só `Go back`.

#### FRT-008 — Easter eggs de nome: revisão `[ ]`
P2 · P · FRT-007
- **O quê:** manter a tabela do ROTEIRO §6. Adicionar: nome proibido `GASTER` → reinicia o jogo (como no Undertale; paródia: `GABEN` → "Half-Life 3 confirmed" e reinicia); `MTTDEW`/`DORITO` etc. Migrar a lista para `data/names.json`.

#### FRT-009 — Settings `[ ]`
P2 · M · CORE-007
- **O quê:** a tela "SETTINGS" com `EXIT`, volume de música/SFX, idioma, tela cheia, escala inteira e controles. Fundo/estilo do Undertale (texto grande, opção selecionada em amarelo).

#### FRT-010 — Disclaimer `[ ]`
P3 · P · BUG-01
- **O quê:** já existe; revisar o fade inicial (depende do BUG-01) e garantir que Z pula.

---

### AUD — Áudio

#### AUD-001 — Música por contexto `[ ]`
P0 · M · D-04
- **O quê:** faixas separadas (originais, no estilo chiptune do Undertale): intro, título/menu, sala da queda (ambiente), Ruínas (exploração), casa da Furiell, batalha comum, Barky (chefe), Furiell (chefe), game over, loja, Snowdinha. Hoje `epic_background_loop.ogg` toca no título, no overworld **e** na batalha.
- **Mapa:** `data/music.json` com faixa por sala/área/encontro, loop points (`loopStart`, `loopEnd`).

#### AUD-002 — Biblioteca de SFX equivalente `[ ]`
P0 · M · D-04
- **O quê:** criar/obter sons **originais** (sfxr/ChipTone/jsfxr/Bfxr ou gravados) cobrindo cada função do Undertale. Lista completa no Apêndice B. Nomes padronizados `assets/sfx/<função>.ogg`.

#### AUD-003 — Sons de navegação corretos `[ ]`
P0 · P · AUD-002
- **O quê:** **mover o cursor** = som curto "squeak"; **confirmar** = "select". Hoje os dois usam `selection_sound.wav` com volumes diferentes.

#### AUD-004 — Cortes e retomadas de música `[ ]`
P1 · P · AUD-001
- **O quê:** a música para no encontro (ENC-002); toca o tema de batalha; na vitória a música para; ao voltar ao overworld, a música da área **recomeça** (o Undertale reinicia a faixa, confirmar) ou continua.

#### AUD-005 — Loop points `[ ]`
P1 · P · AUD-001
- **O quê:** suporte a intro + loop (raylib `Music` com seek manual em `loopStart` ao passar de `loopEnd`).

#### AUD-006 — Fade de música `[ ]`
P2 · P · AUD-001
- **O quê:** `fadeOut(frames)` / `fadeIn(frames)` para cutscenes (a Furiell indo embora, o Senpai Sans aparecendo).

#### AUD-007 — Canais e prioridade `[ ]`
P2 · P · —
- **O quê:** permitir o mesmo som sobreposto (multi-instância com `LoadSoundAlias`), ex.: vozes rápidas e balas.

#### AUD-020 — Auditoria de licenças de áudio `[ ]`
P0 · P · D-04
- **O quê:** registrar a origem de cada arquivo de `assets/music/` e `assets/sfx/` em `assets/LICENSES.md`; marcar o que precisa ser substituído antes do release.

---

### FUR — Mecânica exclusiva: Furrificação

#### FUR-001 — Design do Medidor Furry `[ ]`
P0 · M · D-05
- **O quê:** documento em `docs/furry_meter.md` definindo:
  - **Ganho:** +N% por golpe recebido (N depende da bala/inimigo; hoje são +8 fixos), +bônus em certos ataques "fofos".
  - **Perda:** itens (Duct Tape −15%), save point (−X% ou nada?), ACTs específicos ("Touch Grass").
  - **Estágios:** 0–19 humano-sapo; 20 orelhas; 40 patas; 60 rabo; 80 focinho; 100 = furrificação completa.
  - **HP 0:** game over (BND-010) com furrificação; ao recarregar, o medidor volta ao valor do save + "cicatriz" permanente de +5% (opção (c) da D-05)?
  - **100%:** final especial "Furry Ending" (NAR-013) ou game over imediato?
- **Aceite:** o documento é aprovado e referenciado no ROTEIRO.

#### FUR-002 — Ganho por dano `[ ]`
P0 · P · FUR-001, BHL-005
- **O quê:** aplicar a regra no hit; o valor por bala vem dos dados.

#### FUR-003 — Exibição no HUD estilo "KR" `[ ]`
P0 · P · BUI-005, FUR-001
- **O quê:** em vez de "FURRY: 0%" no meio do HUD, desenhar como o **KARMA** do Sans: label pequeno `FUR` depois da barra de HP e uma porção **rosa/roxa** sobreposta à barra proporcional ao medidor; os números ficam rosados quando > 0. Escondido quando 0%.
- **Por quê:** mantém o HUD 1:1 em lutas normais e reaproveita uma linguagem visual que fã do Undertale reconhece.

#### FUR-004 — Sprites do Pepe por estágio `[ ]`
P1 · G · ART-001, FUR-001
- **O quê:** 6 variantes da spritesheet de overworld (orelhas, patas, rabo, focinho, completo) + variantes do retrato/alma (a alma ganha orelhinhas no estágio 100?).

#### FUR-005 — Reações de NPCs ao estágio `[ ]`
P2 · M · FUR-001, NAR-*
- **O quê:** diálogos condicionais (`furry >= 40`: "Nice ears, newcomer! Welcome to the fandom!"). Furiell fica mais "acolhedora" quanto mais furry.

#### FUR-006 — Furry meter no STAT `[ ]`
P1 · P · MNU-003

---

### NAR — Roteiro e escrita

> Tarefas de **escrita** no `ROTEIRO.md`/arquivos de diálogo. O roteiro atual cobre só 4 cenas do Capítulo 1. Os itens de conteúdo (C1, C2, C3+) dependem destes.

#### NAR-001 — Unificar o lore da intro `[ ]`
P0 · P · —
- **O quê:** há três versões: o ROTEIRO (Gamers vs Furries/Weeaboos, as Flame Wars, a barreira de desodorante vencido + sem Wi-Fi), o `LoreScene.cpp` (Furries vs Gamers, "yiffing") e o `notes.txt` (Furries vs Weeaboos, Frisk furry protagonista). Escolher **o ROTEIRO** como canônico e reescrever as 6 cartas de texto da intro no ritmo do Undertale (frases curtas, ~3 linhas por carta, 9–11 cartas).

#### NAR-002 — Script completo da luta do Barky `[ ]`
P0 · M · D-06
- **O quê:** texto de encontro, Check, todas as respostas de ACT (incluindo combinações e repetições: "* You stare at the zipper again. Barky is out of blush."), flavor texts (default, low HP, poupável), 10–15 falas de bolha, textos de vitória/spare/morte, e a fala ao ser poupado no overworld depois.

#### NAR-003 — Cenas da Furiell (Capítulo 1) `[ ]`
P0 · G · D-06
- **O quê:** escrever o equivalente de toda a jornada da Toriel: o resgate (de quê? da paródia do Flowey?), o tutorial de puzzles, o tutorial de batalha com o "Fursuit Dummy", segurar a mão pelos espinhos (paródia: "pelos fios de RGB soltos"), deixar o Pepe sozinho no corredor longo (teste de independência + ligações no CELL), a casa (pop-tart com glitter, o quarto, o livro, a lareira), a conversa "quando vou poder ir embora?", descer as escadas, a luta final e o desfecho (spare/kill).

#### NAR-004 — Ligações do CELL `[ ]`
P1 · M · NAR-003
- **O quê:** falas de `Say Hello`, `Puzzle Help`, `About Yourself`, `Call Her "Mod"` por sala, mais as ligações forçadas (ex.: "Do you prefer cinnamon or butterscotch?" → "Do you prefer glitter or MORE glitter?").

#### NAR-005 — Textos de exame de objetos das Ruínas `[ ]`
P1 · M · C1-*
- **O quê:** toda placa, parede, decoração e item pegável com texto 1:1 em tom, mantendo as piadas do ROTEIRO (o aviso da Administração das Ruínas, o CRT com tela azul em Comic Sans, as faixas da "Midwest FurFest 2014").

#### NAR-006 — Monstros comuns das Ruínas `[ ]`
P0 · G · C1-020
- **O quê:** para cada inimigo comum (lista em C1-020): nome, Check, 2–3 ACTs, respostas, flavor texts, falas de bolha e condição de spare.

#### NAR-007 — Paródia do Flowey `[ ]`
P1 · M · —
- **O quê:** o ROTEIRO não tem o Flowey. Proposta: **"Trollowey"**, uma flor com cara de trollface que ensina o "friendliness pellets" → "free karma pellets" e tenta te furrificar logo de cara, até a Furiell chegar. Validar se entra.

#### NAR-008 — Cena 4 revisada (saída das Ruínas) `[ ]`
P0 · P · D-06
- **O quê:** reescrever "The Path to Snowdinha" depois da luta com a Furiell: o portão, o corredor escuro com a paródia do Flowey comentando o que você fez, a floresta gelada com aurora de neon e o Senpai Sans na guarita.

#### NAR-010 — Nomes e jargão do sistema `[ ]`
P1 · P · D-07
- **O quê:** definir os nomes paródicos visíveis ao jogador: LV, EXP, DETERMINATION ("GAMER DETERMINATION"?), SAVE, "* But nobody came.", ITEM/STAT/CELL.

#### NAR-011 — Texto do game over `[ ]`
P0 · P · BND-010
- **O quê:** o logo (GAME OVER vs FURRY OVER), a voz e 5–8 frases aleatórias que usam o nome do jogador.

#### NAR-012 — Falas meta (quarta parede) `[ ]`
P2 · M · SAV-005
- **O quê:** falas do Senpai Sans e da paródia do Flowey que reagem a reset, a load depois de morrer, a matar alguém e voltar com load.

#### NAR-013 — Finais `[ ]`
P2 · G · PRG-030, FUR-001
- **O quê:** esboço dos finais: Wholesome (o Pepe volta para o PC e o raid group o recebe), Normie (variações), Toxic Gamer, Furry Ending (100% de furrificação: vira o mascote da convenção, como diz o ROTEIRO §1).

#### NAR-014 — Guia de estilo de escrita `[ ]`
P1 · P · —
- **O quê:** `docs/writing_style.md`: o narrador sempre com "* ", segunda pessoa, frases curtas, no máximo 3 linhas por página, o humor dos memes **sem** depender de slurs ou de conteúdo sexual explícito (o "yiff" da intro atual é um exemplo do que evitar para manter o tom do Undertale), as fontes e vozes por personagem.

---

### C1 — Conteúdo: Capítulo 1 "A Descida ao Mt. Dew" (Ruínas)

> Sequência de salas proposta, espelhando a estrutura das Ruínas do Undertale e encaixando as 4 cenas do ROTEIRO. Itens marcados **(proposta)** precisam ser validados no ROTEIRO antes (NAR-*).

#### C1-001 — Sala da queda `[ ]`
P0 · M · OVW-003, ART-040
- **ROTEIRO Cena 1:** clareira com piso roxo rachado, sacos de salgadinho amassados, latas verdes vazias, um feixe de luz vindo do buraco no teto. O Pepe acorda, levanta, coça a testa: "Ugh... where's my wireless mouse?..." (com retrato/voz do Pepe).
- **Save point** (exceção ao Undertale, onde não há save nessa sala — mantido por ser decisão do roteiro).
- **Detalhes 1:1:** a câmera começa no Pepe deitado na mancha de luz; a animação de levantar (3–4 frames); a música ambiente.

#### C1-002 — Encontro com a paródia do Flowey `[ ]`
P1 · G · NAR-007, BHL-004
- **(proposta)** a primeira "batalha" é o tutorial falso (as balas "free karma pellets"), o Trollowey te prende num círculo de balas, a Furiell aparece e o espanta com uma bola de fogo (paródia: "spray de purpurina"). Inclui o tutorial de movimentação da alma.

#### C1-003 — Entrada das Ruínas `[ ]`
P0 · M · OVW-003, ART-041
- **ROTEIRO Cena 2:** arcos de pedra com as faixas "Midwest FurFest 2014 - Welcome!", a escadaria dupla, as folhas (embalagens) e o save point. A Furiell se apresenta (NAR-003).

#### C1-004 — Sala do quadro de avisos e do CRT `[ ]`
P0 · P · OVW-011
- **ROTEIRO Cena 2:** a placa "NOTICE FROM RUINS ADMINISTRATION..." (já existe no código como "SIGN") e o "Ancient CRT Computer" (BSOD com mensagem em Comic Sans).

#### C1-005 — Puzzle tutorial de interruptores `[ ]`
P1 · M · OVW-012
- **(proposta)** a Furiell mostra os interruptores marcados ("os amarelos são os certos"); paródia: interruptores de RGB que a Furiell rotulou com glitter.

#### C1-006 — Sala do boneco de treino `[ ]`
P0 · M · BFL-001, NAR-003
- **(proposta)** "Fursuit Dummy": o tutorial de batalha. A Furiell sugere "conversar" (ACT → Talk). Todos os resultados possíveis (falar, bater, fugir, poupar) com reações dela, como no Undertale.

#### C1-007 — Labirinto de espinhos (de mãos dadas) `[ ]`
P1 · M · CORE-011
- **(proposta)** a Furiell guia o Pepe pelo caminho seguro, com uma cutscene de movimento scriptado.

#### C1-008 — Corredor longo + teste de independência `[ ]`
P1 · M · CORE-011, MNU-004
- **(proposta)** a Furiell manda esperar e sai; o celular é entregue; ligações (NAR-004); atrás da pilastra, a Furiell "escondida" (fursuit aparecendo).

#### C1-009 — Sala do Barky (primeiro encontro de verdade) `[ ]`
P0 · M · NAR-002, CORE-011, BHL-014
- **ROTEIRO Cena 3:** uivo ecoando, o Barky pula do alto e cai agachado de braços cruzados, a fala de apresentação, a batalha (ACT-002, BHL-014). Depois: Barky poupado vira NPC ("Shadow Fang Pack" recrutando o Pepe); Barky morto deixa só poeira + flag.
- **Hoje:** existe uma versão de teste disso em `OverworldScene` (NPC + "EXIT" que dispara a luta). Migrar para dados.

#### C1-010 — Salas de puzzle intermediárias `[ ]`
P1 · G · OVW-012
- **(proposta)** 4–6 salas: empurrar pedras (pedra que fala → paródia "a pedra que é mod do servidor"), piso que racha (labirinto de folhas), a tigela de doces ("Take one" → "Take one Dorito"; pegar 4 → quebra a tigela), a sala dos sapos que dão dicas (paródia: sapos que são o próprio Pepe em formas diferentes: "Wojak", "Doomer"), a sala do ratinho e do queijo (o queijo grudado na mesa → "a pizza fria grudada no teclado").

#### C1-011 — Venda paródica (NFT Bake Sale) `[ ]`
P2 · M · PRG-004

#### C1-012 — Paródia do Napstablook `[ ]`
P1 · M · NAR-006
- **(proposta)** "Napstablurk", um fantasma "lurker" que nunca posta e faz lo-fi beats; luta sem dano (as "lágrimas" viram mensagens "seen"); ACTs `Cheer`, `Flirt`, `Like their post`.

#### C1-013 — Casa da Furiell `[ ]`
P0 · G · NAR-003, ART-042
- **(proposta)** sala de estar (lareira, poltrona, livro de piadas → livro de "fanfics de qualidade duvidosa"), cozinha, corredor com espelho, quarto do Pepe (a cama — dormir cura e o pop-tart aparece no chão), o quarto dela, a escadaria para o porão.

#### C1-014 — Porão e luta com a Furiell `[ ]`
P0 · GG · NAR-003, BHL-004
- **(proposta)** o corredor do porão com as falas dela ("go upstairs"), a luta: o tema próprio, ataques de "bolas de pelo" e "glitter", as **balas desviam do Pepe quando o HP está baixo** (como a Toriel), o spare repetido até ela desistir, os diálogos que avançam a cada spare, o final pacifista (abraço, ela volta para casa) e o final de kill (ela vira poeira com fala final).

#### C1-015 — Saída das Ruínas `[ ]`
P0 · M · NAR-008
- **ROTEIRO Cena 4 (revisada):** o portão de ferro se abre e revela a floresta gelada com aurora de neon; o Senpai Sans encostado na guarita à distância.

#### C1-020 — Bestiário das Ruínas `[ ]`
P0 · G · NAR-006, ART-011
- **(proposta)** paródias espelhando os comuns do Undertale (cada um com sprite, idle, 2–3 padrões, ACTs, spare):
  | Undertale | nUndertale (proposta) | Conceito | ACTs |
  | :--- | :--- | :--- | :--- |
  | Froggit | **Frogposter** | sapo que só responde com reaction image | Compliment ("Based"), Threaten |
  | Whimsun | **Shysona** | fursona tímido que pede desculpa por existir | Console, Terrorize |
  | Moldsmal | **Modsmal** | moderador de Discord gelatinoso que só "deita" e bane | Imitate, Flirt |
  | Loox | **Lurkeye** | olho único que só observa o chat | Don't Pick On, Pick On |
  | Vegetoid | **Soytoid** | legume com cara de soyjak | Dinner (balas verdes curam), Devour |
  | Migosp | **Reply Guy** | sempre "well, actually"; sozinho, fica tranquilo | Talk |
  | Training Dummy | **Fursuit Dummy** | manequim com cabeça de fursuit | Talk |
  | Napstablook | **Napstablurk** | ver C1-012 | Cheer, Flirt |
- **Encontros:** tabela por sala, incluindo os combos ("Frogposter and co.").

#### C1-030 — Capítulo 1 jogável do início ao fim `[ ]`
P0 · GG · C1-001..C1-015
- **Aceite:** do novo jogo até a saída das Ruínas sem usar debug; save/load em todos os save points; as três rotas possíveis dentro do capítulo (spare tudo, misto, matar tudo com "nobody logged in").

---

### C2 — Conteúdo: Snowdinha

> O ROTEIRO só apresenta o Senpai Sans e o Papyrus Streamer. Detalhar depois do C1.

#### C2-001 — Encontro com o Senpai Sans na ponte `[ ]`
P1 · G · NAR-*, GFX-005
- **(proposta)** o portão "grande demais", os passos atrás, "don't you know how to greet a new pal? turn around and shake my hand." → a almofada de pum vira **air horn MLG**; a fonte comic; o waifu dakimakura sempre com ele; "heh... my jokes are bare bones-desu, ne?".

#### C2-002 — Papyrus Streamer e puzzles `[ ]`
P1 · GG · OVW-012
- **ROTEIRO:** grades de laser de luz de discoteca, pisos de CAPTCHA obrigatório, teste de digitação rápida. Cada puzzle com a "live" do Papyrus (contador de 3 viewers, chat falso rolando).

#### C2-003 — Bestiário da Snowdinha `[ ]`
P2 · G · NAR-*
- **(proposta)** Snowdrake → **Punsona**; Ice Cap → **Hat Kid** (o chapéu é a personalidade); Doggo → **Doge** ("much danger, very move"); Lesser Dog → **Lesser Doge** (o pescoço cresce a cada carinho); Greater Dog → **Greater Doge**; Dogamy/Dogaressa → **casal de dogsonas**; Jerry → **Jerry, o Reply Guy da Snowdinha**.

#### C2-004 — Cidade da Snowdinha `[ ]`
P2 · GG · PRG-003
- **(proposta)** a loja, a pousada, o bar (Grillby's → "Grillby's LAN House"), a biblioteca com o erro de digitação ("Librarby"), a casa dos irmãos esqueletos, a caixa dimensional.

#### C2-005 — Luta com o Papyrus Streamer `[ ]`
P1 · GG · BHL-010
- **(proposta)** alma azul, ossos com "!sub" e "PogChamp", o ataque especial que vira um cachorro roubando o osso → **o cachorro roubando o cabo de rede da live**; "captura" se perder (vai para o "galpão de streaming"); o encontro (date) → **collab stream**.

---

### C3+ — Conteúdo: áreas futuras

> Só esqueleto; cada área precisa de um ROTEIRO próprio antes de virar item detalhado.

| Área Undertale | Proposta nUndertale | Chefe/personagem (proposta) | Mecânica nova |
| :--- | :--- | :--- | :--- |
| Waterfall | **Weebfall** (território dos Weeaboos, cachoeiras de Ramune) | **Undyne-chan** (cavaleira de anime armada de katana de papelão); **Tem → "Temmie" vira "Uwu-mie"** | alma verde (BHL-011), lanças, perseguições |
| Hotland | **Hotland Data Center** | **Alphys, a Mod**; **Mettatwitch** (robô influencer) | alma amarela (BHL-012), quiz show, puzzles de esteira |
| — | **Spider Discord Server** | **Muffet** → **Muffet, a aranha do NFT** | alma roxa (BHL-013) |
| Core | **Core Overclock** | — | emboscadas |
| New Home | **Convention Center** | **King Asgore** → **Asgrr, presidente da convenção** | luta final |
| — | — | paródia do Flowey como chefe final da rota Normie | — |

Para cada área: `C<n>-001 Roteiro`, `C<n>-002 Mapas`, `C<n>-003 Bestiário`, `C<n>-004 Chefes`, `C<n>-005 Música`, `C<n>-006 Puzzles`.

---

### ART — Arte (lista de assets)

> Toda arte em pixel art nativa (escala 1×) e desenhada a 2× na tela. Paleta: personagens do overworld coloridos e limitados; **inimigos de batalha em branco/preto** (com cor só quando o Undertale também usa).

#### ART-001 — Spritesheet do Pepe (overworld) `[ ]`
P0 · M · GFX-002
- **O quê:** ~19×29 nativo (o mesmo do Frisk e do `pepe_char.bmp` atual); 4 direções × (parado + ciclo de 4 frames para cima/baixo, 2 frames para os lados), como o Frisk. Frames extras: levantando do chão, deitado. PNG com alfa.
- **Hoje:** o `pepe_char.bmp` tem poucos frames e usa magenta como transparência.

#### ART-002 — Retratos de diálogo `[ ]`
P1 · G · TXT-005
- **O quê:** Pepe (para as falas dele), Furiell (6–8 expressões), Senpai Sans (4–6), Papyrus (6+), Barky (3–4). ~50×50 nativo (confirmar).

#### ART-010 — Sprite de batalha do Barky `[ ]`
P0 · M · GFX-002
- **O quê:** branco sobre preto, ~60×60 nativo (≈ 120 px na tela), em partes separadas (cabeça, orelhas, corpo com jaqueta de couro falso e correntes de plástico, rabo) para o idle; frame de dano, frame "corado" (depois do Cringe Check), frame poupado.

#### ART-011 — Sprites do bestiário das Ruínas `[ ]`
P0 · G · C1-020

#### ART-012 — Balas `[ ]`
P0 · M · BHL-014
- **O quê:** "UWU" (letras em pixel art, não texto), lata de Dew, onda de uivo, rabo, osso (genérico), bola de pelo, glitter; cada uma com hitbox definida no JSON.

#### ART-020 — Botões de batalha `[ ]`
P0 · M · BUI-006
- **O quê:** 4 botões × 2 estados (laranja/amarelo), 55×21 nativo (110×42 na tela), com as letras na mesma "fonte grossa" do Undertale (desenhadas à mão no sprite).

#### ART-021 — Alma `[ ]`
P0 · P · BUI-007
- **O quê:** a alma inteira, a rachada, 4–6 estilhaços (ou tufos de pelo, BND-010), a alma correndo (fuga), as variantes de cor por modo.

#### ART-022 — Alvo do FIGHT e barra `[ ]`
P0 · P · FGT-001

#### ART-023 — Animação de corte `[ ]`
P0 · P · FGT-004

#### ART-024 — Nuvenzinhas de poeira e partículas de spare `[ ]`
P1 · P · MRC-002

#### ART-030 — Balão "!" `[ ]`
P1 · P · ENC-001

#### ART-031 — Save point (estrela + garrafa) `[ ]`
P0 · P · SAV-001

#### ART-040 — Tileset das Ruínas `[ ]`
P0 · GG · OVW-003
- **O quê:** paredes e piso roxos (a paleta das Ruínas), pilares, arcos, escadas, folhas (embalagens), interruptores, espinhos, portas, faixas da FurFest, CRT, placas, a casa da Furiell.

#### ART-041 — Fundo da intro e telas de título `[ ]`
P1 · M · FRT-002
- **O quê:** refazer as cartas da intro em 400×220 (hoje 398×214) com a paleta sépia limitada do Undertale; a imagem alta da queda.

#### ART-042 — Arte dos fundos do menu Continue `[ ]`
P2 · M · FRT-005

#### ART-050 — Ícone e logo `[ ]`
P2 · P · —
- **O quê:** o `icon.png` em 16/32/48/256 (`.ico` multi-tamanho para o executável); o logo "nUNDERTALE" com a fonte Monster Friend.

---

### L10N — Localização

#### L10N-001 — Loader de idiomas `[ ]`
P1 · M · TXT-010
- **O quê:** `data/dialogue/<lang>/*.json` com fallback para `en`; troca de idioma em runtime pelo Settings.

#### L10N-002 — PT-BR completo `[ ]`
P2 · GG · L10N-001
- **O quê:** tradução de todos os textos, adaptando as piadas (não traduzir literal). Conferir se a fonte principal tem os acentos (á, ã, ç…) — a Determination Mono tem cobertura limitada; pode exigir glifos extras (GFX-003).

#### L10N-003 — Medição de overflow `[ ]`
P2 · P · TXT-002
- **O quê:** um script/teste que renderiza todo texto de todos os idiomas e acusa os que passam de 3 linhas na caixa.

---

### QA — Ferramentas de debug, testes e CI

#### QA-001 — Modo debug `[x]`
P0 · P · REPO-003
- **O quê:** `-DNUNDERTALE_DEBUG` ativa: F1 overlay (FPS, sala, posição, estado da cena, flags), F2 hitboxes, F3 menu de debug. Nenhum atalho de debug existe no release.

#### QA-002 — Overlay de hitboxes `[ ]`
P0 · P · QA-001
- **O quê:** desenha as colisões da sala, a caixa de interação do Pepe, a hitbox da alma e de cada bala.

#### QA-003 — Menu de debug `[ ]`
P1 · M · QA-001
- **O quê:** warp para qualquer sala, iniciar qualquer encontro, editar HP/LV/gold/furry/flags, god mode (sem dano), pular o turno inimigo, velocidade 0,25×/0,5×/2× da lógica.

#### QA-004 — Tela de comparação com a referência `[ ]`
P1 · P · QA-001
- **O quê:** a tecla F6 alterna a sobreposição de `docs/reference/*.png` (prints do Undertale) com 50% de opacidade sobre o canvas, para conferir o layout pixel a pixel.

#### QA-005 — Battle sandbox `[ ]`
P1 · M · BHL-004
- **O quê:** uma cena só para testar padrões de ataque em loop, com a escolha do padrão e o ajuste de parâmetros ao vivo.

#### QA-010 — Testes unitários `[ ]`
P1 · M · REPO-003
- **O quê:** vendorizar `doctest` (single header); `tests/` com: roundtrip do `GameState`, tabela de LV, fórmula de dano, quebra de linha/recuo, parser de markup, condição de spare, carregamento de todos os JSONs de `data/` (schema).

#### QA-011 — Testes da fórmula de dano `[ ]`
P1 · P · FGT-003, QA-010

#### QA-012 — Validação de dados no build `[ ]`
P2 · M · QA-010
- **O quê:** um passo que valida todos os JSON (IDs de itens/inimigos/salas/sons referenciados existem, chaves de texto existem em `en`).

#### QA-013 — CI no GitHub Actions `[ ]`
P2 · M · REPO-005
- **O quê:** um workflow Windows que roda `tools/setup_vendor.ps1`, compila o release, roda os testes e publica o `.zip` como artifact.

#### QA-014 — Crash handler `[ ]`
P3 · P · —
- **O quê:** capturar exceções/sinais, gravar `crash_reports/<data>.txt` (a ideia existia na versão Python) com a cena, a sala e a pilha.

#### QA-015 — Checklist de "feeling" `[ ]`
P1 · P · —
- **O quê:** `docs/feel_checklist.md` com os itens que só se verificam jogando lado a lado com o Undertale: velocidade da alma, ritmo do texto, tempo da transição de encontro, peso do game over, resposta dos menus (sem input lag), som de cada ação. Rodar a cada marco.

---

### DST — Distribuição

#### DST-001 — Build de release e pasta de saída `[ ]`
P2 · P · REPO-003
- **O quê:** `build.bat release` gera `dist/nUndertale/` com `nUndertale.exe` (`-mwindows`, sem console, com ícone via `.rc`), `assets/`, `data/` e as licenças. Seguindo a convenção do projeto, o artefato final fica em `C:\Users\delux\OneDrive\Documentos\Programas\C\` (a stack C/C++).

#### DST-002 — Empacotamento `[ ]`
P3 · P · DST-001
- **O quê:** `.zip` versionado (`nUndertale-v0.x.zip`); opcional: itch.io (butler).

#### DST-003 — Créditos e avisos `[ ]`
P1 · P · D-04
- **O quê:** a tela de créditos + `CREDITS.md` com as fontes, sons e músicas de terceiros e o aviso de fan parody não afiliada à Toby Fox/Undertale.

---

## 6. Roadmap por marcos

### M0 — Fundação (antes de qualquer conteúdo)
`REPO-000`, `REPO-001`, `REPO-003`, `REPO-007`, `BUG-01`, `CORE-001`, `CORE-002`, `CORE-004`, `CORE-009`, `CORE-010`, `GFX-001`, `GFX-002`, `GFX-003`, `GFX-004`, `QA-001`, `QA-002`, `D-01`, `D-02`, `D-04`, `D-05`, `D-06`.
**Pronto quando:** o loop é fixo a 30 Hz, a batalha volta para o overworld preservado, as fontes ficam pixel-perfect e o debug overlay funciona.

### M1 — Vertical slice da batalha 1:1 (Barky)
`TXT-001`..`TXT-004`, `TXT-008`, `BUI-001`..`BUI-009`, `BFL-001`..`BFL-003`, `BFL-005`, `BFL-006`, `FGT-001`..`FGT-007`, `ACT-001`, `ACT-002`, `ITM-001`..`ITM-004`, `MRC-001`..`MRC-003`, `BHL-001`..`BHL-007`, `BHL-014`, `BND-001`..`BND-004`, `BND-010`, `ENC-002`, `FUR-001`..`FUR-003`, `NAR-002`, `NAR-011`, `ART-010`, `ART-012`, `ART-020`..`ART-023`, `AUD-002`, `AUD-003`, `QA-004`.
**Pronto quando:** sobrepondo um print da batalha do nUndertale ao print do Undertale, o layout coincide; a luta com o Barky tem o mesmo ritmo e a mesma sensação de uma luta das Ruínas.

### M2 — Overworld 1:1
`OVW-001`..`OVW-011`, `GFX-010`, `TXT-005`..`TXT-007`, `TXT-010`, `MNU-001`..`MNU-003`, `SAV-001`..`SAV-004`, `PRG-001`, `PRG-002`, `PRG-020`, `ENC-001`, `ENC-003`, `CORE-003`, `CORE-005`, `CORE-006`, `ART-001`, `ART-031`, `ART-040` (parcial), `AUD-001`.
**Pronto quando:** dá para andar por 3 salas reais, conversar, salvar, abrir o menu C, cair num encontro aleatório e voltar com tudo preservado.

### M3 — Front-end 1:1
`FRT-001`..`FRT-007`, `NAR-001`, `ART-041`, `CORE-007`, `CORE-012`.

### M4 — Capítulo 1 completo
`C1-*`, `NAR-003`..`NAR-008`, `NAR-014`, `MNU-004`, `OVW-012`, `PRG-030`, `ENC-004`, `BHL-008`, `FUR-004`, `FUR-005`, `ART-002`, `ART-011`, `ART-040`, `AUD-001` (todas as faixas do C1), `AUD-020`, `DST-003`.
**Pronto quando:** `C1-030` passa.

### M5 — Snowdinha
`C2-*`, `BHL-010`, `GFX-005`, `PRG-003`, `MNU-005`.

### M6+ — Áreas futuras, finais, localização e release
`C3+`, `NAR-012`, `NAR-013`, `SAV-005`, `L10N-*`, `QA-013`, `DST-*`.

---

## 7. Apêndices

### Apêndice A — Schema sugerido de inimigo (`data/enemies/barky.json`)

```json
{
  "id": "barky",
  "name": "Barky",
  "hp": 30, "atk": 4, "def": 2,
  "exp": 20, "gold": 10,
  "sprite": "sprites/enemies/barky.json",
  "offset": [0, 0],
  "voice": "barky",
  "check": "enemy.barky.check",
  "acts": [
    { "id": "cringe_check", "label": "Cringe Check", "text": "enemy.barky.act.cringe",
      "effects": { "mercy": 50, "setFlag": "battle.barky.blushed" },
      "response": "enemy.barky.reply.cringe" },
    { "id": "offer_dew", "label": "Offer Dew", "text": "enemy.barky.act.dew",
      "requires": { "item": "mtn_dew_warm", "consume": false },
      "effects": { "mercy": 100 }, "response": "enemy.barky.reply.dew" },
    { "id": "anime_lore", "label": "Anime Lore", "text": "enemy.barky.act.lore",
      "effects": { "skipNextAttack": true } }
  ],
  "spareWhen": { "mercy": 100 },
  "flavor": {
    "default": ["enemy.barky.flavor.1", "enemy.barky.flavor.2"],
    "lowHp": ["enemy.barky.flavor.lowhp"],
    "spareable": ["enemy.barky.flavor.spareable"],
    "flag:battle.barky.blushed": ["enemy.barky.flavor.blushing"]
  },
  "dialogue": {
    "default": ["enemy.barky.bubble.1", "enemy.barky.bubble.2"],
    "afterHit": ["enemy.barky.bubble.hit"]
  },
  "patterns": [
    { "id": "uwu_rain", "weight": 2, "params": { "speed": 3, "density": 0.4, "damage": 3, "furry": 6 } },
    { "id": "dew_bounce", "weight": 1, "params": { "cans": 3, "damage": 4, "furry": 4 } },
    { "id": "tail_swipe", "when": "hp<50%", "params": { "damage": 5, "furry": 10 } }
  ],
  "canFlee": false,
  "music": "music/boss_barky.ogg"
}
```

### Apêndice B — SFX necessários (equivalentes originais)

| Função | Nome sugerido | Onde |
| :--- | :--- | :--- |
| Mover o cursor | `menu_move` | todos os menus |
| Confirmar | `menu_select` | todos os menus |
| Voz do narrador | `voice_default` | caixas "* " |
| Vozes de personagem | `voice_<npc>` | diálogos |
| Bip do encontro | `encounter_beep` | ENC-002 (3×) |
| Alma descendo para o botão | `encounter_fall` | ENC-002 |
| "!" | `alert` | ENC-001 |
| Corte (ataque) | `slash` | FGT-004 |
| Hitmarker (crítico) | `hitmarker` | FGT-008 |
| Dano no inimigo | `enemy_hit` | FGT-005 |
| Dano na alma | `hurt` | BHL-007 |
| Cura | `heal` | itens, save |
| Save | `save` | SAV-002 |
| Spare | `spare_poof` | MRC-001 |
| Vaporizar | `vaporize` | BND-003 |
| Level up | `level_up` | BND-004 |
| Fuga | `flee` | MRC-002 |
| Alma rachando | `soul_crack` | BND-010 |
| Alma estilhaçando | `soul_shatter` | BND-010 |
| Porta/transição | `door` | OVW-008 (se houver) |
| Interruptor | `switch` | puzzles |
| Espinhos baixando | `spikes` | puzzles |
| Folhas/embalagens | `step_crunch` | OVW-014 |
| Celular | `phone_ring`, `phone_dial` | MNU-004 |
| Título aparecendo | `intro_noise` | FRT-003 |
| Fade branco do nome | `name_swoosh` | FRT-007 |
| Pulo de sapo/ribbit | `ribbit` | easter eggs, Pepe |
| Air horn MLG | `airhorn` | Senpai Sans |

### Apêndice C — Arquivos do código atual mais afetados

| Arquivo | O que muda |
| :--- | :--- |
| `src/combat/BattleSystem.cpp/.hpp` | reescrita quase total (BFL-001); dividir em `BattleBox`, `BattleHud`, `BattleMenu`, `FightBar`, `EnemyTurn`, `BattleText` |
| `include/combat/Enemy.hpp` | vira `EnemyDef` (dados) + `EnemyInstance` (estado na luta) |
| `include/combat/Bullet.hpp` | vira um sistema de balas com sprite, hitbox por tipo e pooling |
| `src/scenes/BattleScene.cpp` | recebe o `Encounter` por parâmetro; push/pop na pilha |
| `src/scenes/OverworldScene.cpp` | vira um `RoomScene` genérico carregado do Tiled |
| `src/ui/DialogBox.cpp/.hpp` | vira o `TextBox` sobre o motor de texto (TXT-001) |
| `src/core/Game.cpp` | loop fixo a 30 Hz (CORE-001), F4, segurar ESC |
| `src/core/SceneManager.cpp/.hpp` | pilha com transições plugáveis (CORE-002/003) |
| `include/core/GameContext.hpp` | `PlayerData` → `GameState` serializável (CORE-004) |
| `include/core/Constants.hpp` | paleta oficial, medidas de batalha, velocidades por quadro |
| `src/services/ResourceService.cpp` | fontes por `FontId` com escala inteira; spritesheets |
| `src/services/AudioService.cpp` | loop points, fade, multi-instância, BUG-13 |
| `src/services/InputService.cpp` | ações abstratas, buffer entre ticks, gamepad |
| `src/scenes/TitleScene.cpp`, `NameSelectScene.cpp`, `LoreScene.cpp`, `InstructionsScene.cpp` | fluxo e layout do Undertale (FRT-*) |
| `src/entities/Player.cpp` | velocidade por quadro sem normalização, spritesheet nova |
| `src/entities/Interactable.cpp` | o save point em sprite, os tipos genéricos de objeto vindos do Tiled |
