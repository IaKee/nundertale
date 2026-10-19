# Catálogo Mestre e Exaustivo de Efeitos Sonoros (SFX) — nUndertale

> **Objetivo:** Fornecer o inventário completo, exaustivo e categorizado de **todos os efeitos sonoros** necessários para atingir 100% de paridade com o áudio do *Undertale* original, além de todos os efeitos exclusivos da paródia *nUndertale* (Gamer vs Furry, LORE §27.2 e BACKLOG Appendix B).
>
> **Status dos arquivos:** A maioria dos áudios atuais em `assets/sfx/` são placeholders provisórios ou sintetizados proceduralmente. Este documento serve como guia definitivo de caça, gravação, síntese e substituição de assets.

---

## 1. Padrões e Especificações Técnicas dos Áudios

Todo asset de efeito sonoro adicionado ao projeto deve respeitar estritamente as regras de engine (`raylib` + `AudioService`):

| Parâmetro | Requisito Obrigatório | Motivo Técnico |
| :--- | :--- | :--- |
| **Formato** | **OGG Vorbis** (padrão) ou **WAV PCM 16-bit** (somente para SFX < 0.5s) | Reduz tamanho da build sem perda de fidelidade; carrega instantaneamente no cache. |
| **Canais** | **Mono (1 canal)** | Efeitos sonoros 2D em pixel art retrô não exigem estéreo; mono economiza 50% de RAM e CPU. |
| **Taxa de Amostragem** | **44.100 Hz (44.1 kHz)** | Evita resampleamento em tempo real pela raylib/WASAPI/ALSA. |
| **Volume / Ganho** | **Pico em -3 dBFS** (Normalizado com margem) | Evita clipping/distorção digital quando múltiplos sons tocam simultaneamente via `LoadSoundAlias`. |
| **Silêncio Inicial** | **0 ms (Zero Trim)** | **Crítico:** corte absoluto de qualquer silêncio no início da forma de onda para não causar sensação de input lag. |
| **Diretório Final** | `assets/sfx/<nome_padronizado>.ogg` | Padronização direta referenciada em código sem caminhos absolutos. |

---

## 2. Diagnóstico dos Arquivos Atuais em `assets/sfx/`

| Arquivo Atual | O que está fazendo hoje | Origem / Situação | Ação Necessária |
| :--- | :--- | :--- | :--- |
| `menu_move.wav` | Som de mover o cursor nos menus | Gerado por `tools/gen_placeholder_sfx.js` | Placeholder OK provisoriamente; pode receber versão final com mais textura retrô. |
| `menu_select.wav` | Som de confirmação / seleção | Gerado por `tools/gen_placeholder_sfx.js` | Placeholder OK provisoriamente; polir para versão final. |
| `heal.wav` | Som de cura com itens / save | Gerado por `tools/gen_placeholder_sfx.js` | Placeholder OK provisoriamente (arpeggio triangular). |
| `selection_sound.wav` | Som antigo de seleção | Desconhecido (não mais usado) | **Remover ou arquivar** (já substituído por menu_move e menu_select). |
| `sans_type.wav` | Voz do Sans / easter egg de nome | **Rip provável do Undertale original** | **Substituição obrigatória imediata** por sintetização original antes de qualquer release. |
| `u_activate.wav` | Ding de spare, save e confirmação de nome | Origem desconhecida / genérica | Substituir por `save_can.ogg` e `bell.ogg`. |
| `type.ogg`, `type_long.ogg` | Voz padrão da caixa de diálogo | Origem desconhecida | Substituir por `voice_default.ogg` e variantes originais. |
| `u1.ogg` até `u12.ogg` | Balbucio / vozes de personagens | Origem desconhecida | Substituir pela biblioteca de vozes tipográficas padronizadas (`voice_<npc>`). |
| `n.ogg` | Tremor do 'n' na TitleScene | Origem desconhecida | Substituir por `intro_noise.ogg`. |
| `areusure.ogg` | Placeholder de tela de derrota | Meme / áudio externo | Substituir pelo som oficial de GAMER OVER / FURRY OVER. |
| `cartoon_slip.ogg` | Placeholder de queda / animação cômica | Áudio de desenho animado externo | Substituir por `pomf.ogg` ou `flee.ogg`. |
| `pipipipopopo_backwards.ogg` | Placeholder de ataque / entrada do Barky | Meme / áudio invertido | Substituir por `howl.ogg` e `slash.ogg`. |

---

## 3. Catálogo Exaustivo de Efeitos Sonoros

### 3.1 Interface de Usuário & Menus (UI)

Sons fundamentais de navegação, escolhas e feedback de sistema do jogador.

| Arquivo Final | Equivalente Undertale | Onde Toca no Jogo | Descrição Sonora (Timbre, Forma de Onda, ADSR) | Como Sintetizar (Bfxr / ChipTone) | Termos de Busca (Freesound / CC0) | Prio. |
| :--- | :--- | :--- | :--- | :--- | :--- | :---: |
| `menu_move.wav` | `snd_squeak` | Cada troca de opção no menu, inventário e batalha | Chirp/squeak agudo curtíssimo (~35ms). Onda quadrada com pitch subindo rapidamente de 1250 Hz para 1600 Hz. Decaimento exponencial imediato. | Bfxr: Preset *Jump* ou *Blip*, encurtar Decay para 0.05, pitch alto, High-pass filter leve. | `pixel chirp`, `menu cursor 8bit`, `ui blip high` | **P0** |
| `menu_select.wav` | `snd_select` | Confirmar qualquer botão, item ou escolha | Bi-tom ascendente claro e agradável (~120ms). Duas ondas quadradas ou triangulares: 1ª nota em ~660 Hz (E5), 2ª nota em ~990 Hz (B5). | Bfxr: Preset *Pickup/Coin*, envelope curto, sem vibrato, duas frequências em legato rápido. | `ui confirm 8bit`, `retro menu select`, `blip confirm` | **P0** |
| `bell.ogg` | `snd_bell` | Nome do monstro fica amarelo no ACT; ding de aviso; confirmação especial | Sino metálico estilo hotel concierge / recepção (~400ms). Timbre brilhante, ressonância longa e cristalina com harmônicos agudos (~2500 Hz). | Foley real: bater em taça de vidro fino ou campainha de balcão. ChipTone: chime senoidal puro com decay longo. | `hotel desk bell`, `desk bell ding`, `service bell` | **P0** |
| `menu_error.ogg` | `snd_cantselect` | Tentar selecionar algo inválido ou limite de inventário | Buzzer retrô suave, não irritante (~80ms). Onda dente-de-serra ou ruído tonal grave em ~150 Hz com pitch bend levemente descendente. | Bfxr: Preset *Hit/Hurt*, mudar para Sawtooth, freq base 150 Hz, sem sweep, decay muito curto. | `retro buzzer`, `negative blip 8bit`, `error beep` | **P1** |
| `item_buy.ogg` | `snd_buyitem` | Ao comprar qualquer item na loja de conveniência | Tintilar de moedas / caixa registradora retrô (~250ms). Dois estalos metálicos rápidos seguidos de um chime agudo curto. | Gravar duas moedas reais se chocando ou Bfxr com arpeggio ultra rápido de onda quadrada. | `cash register retro`, `coin sound effect`, `buy item chiptune` | **P1** |
| `flee.ogg` | `snd_escaped` | Ao fugir com sucesso da batalha (MERCY -> Flee) | Passos rápidos de desenho animado / corrida cômica (~600ms). Sequência de 4 a 6 "pats" rápidos e ocos com frequência levemente ascendente. | Gravar sapatos leves batendo rápido numa mesa de madeira, ou Bfxr preset *Random* modulado em pitch. | `cartoon running feet`, `fast steps cartoon`, `retro scramble` | **P0** |
| `save_can.ogg` | `snd_save` | Ao tocar no Ponto de Save (Garrafa de Mountain Dew) | **Específico nUndertale:** Som ultra nítido de lata de refrigerante abrindo: estalo de anel de alumínio (*clack*) seguido pelo chiado de gás carbônico (*tsssss*, ~600ms). | Foley real: abrir lata de refri perto do microfone com pop-filter, ou unir um *click* metálico a ruído branco filtrado. | `soda can open`, `can pop fizz`, `opening soda can` | **P0** |

---

### 3.2 Vozes Tipográficas de Diálogo (Dialogue Voice Beeps)

No estilo de *Undertale*, cada caractere impresso na caixa de diálogo reproduz um pequeno "beep" característico, dando personalidade e prosódia às falas sem necessidade de dublagem real.

| Arquivo Final | Equivalente Undertale | Personagem Associado | Descrição Sonora (Timbre, Frequência, Caráter) | Receita de Síntese / Criação | Prio. |
| :--- | :--- | :--- | :--- | :--- | :---: |
| `voice_default.ogg` | `snd_text` / `snd_txtmsg` | Narrador, caixas de diálogo gerais, NPCs comuns | Onda quadrada curta (~45ms), pitch estável em 350 Hz. Seco, amigável e neutro. Ataque imediato, decay linear suave. | Sintetizador de onda quadrada pura, LPF cortando acima de 2500 Hz para evitar estalo no cone. | **P0** |
| `voice_sans.ogg` | `snd_txtsans` | Senpai Sans (caveira preguiçosa com dakimakura) | Tom oco, rouco e grave (~60ms). Onda quadrada com pulso largo (75% duty cycle), frequência em torno de 130 Hz (C3). Som de "osso oco". | Onda quadrada grave com ressonância nos médios-graves e decaimento um pouco mais longo. | **P1** |
| `voice_papyrus.ogg` | `snd_txtpap` | Papyrus (fursuit de papelão com selfie-stick) | Blip médio-agudo, staccato rítmico espalhafatoso (~40ms). Pitch em ~520 Hz com harmônicos proeminentes estilo dente-de-serra. | Onda dente-de-serra leve com filtro passa-faixa acentuando 500 Hz a 1 kHz. | **P1** |
| `voice_furiell.ogg` | `snd_txttoriel` | Furiell (mãe cabra em fursuit de avental) | Tom aveludado, carinhoso, doce e maternal (~50ms). Onda senoidal ou triangular arredondada em ~260 Hz (C4), sem aspereza. | Onda senoidal com saturação de fita suave para calor acústico, ataque e release de 5ms. | **P0** |
| `voice_barky.ogg` | `snd_txtmonster` | Barky (lobo emo com jaqueta de spikes) | Pequeno "latido/blip" agudo e anasalado (~50ms). Onda de pulso rápida com pitch descendente sutil (450 Hz -> 380 Hz). | Onda de pulso com sweep rápido descendente, timbre juvenil e estridente. | **P0** |
| `voice_floofy.ogg` | `snd_txtflowey` | Floofy (flor com cabeça de mascote fofo) | Blip inocente, artificial e infantil (~40ms). Frequência aguda em ~620 Hz. *Variação Sinistra:* versão pitch-shifted para 180 Hz com distorção. | Onda triangular aguda limpa; para a versão sinistra, diminuir o pitch em -12 semitons e aplicar bitcrusher. | **P0** |
| `voice_sharkdyne.ogg` | `snd_txtund` | Sharkdyne (tubarão de arpão, heroína caótica) | Ataque áspero e enérgico (~45ms). Onda dente-de-serra com leve saturação em ~290 Hz, soando agressiva e determinada. | Sawtooth em D4 com distorção de overdrive leve para soar cortante. | **P1** |
| `voice_alphys.ogg` | `snd_txtalphys` | Alphys (moderadora de Discord nervosa) | Cliques de teclado mecânico ou blips rápidos e trêmulos (~30ms, ~550 Hz), simulando digitar freneticamente no Discord. | Gravação de switch de teclado mecânico Blue/Brown ou blip ultra-curto com vibrato de frequência. | **P1** |
| `voice_mettapaw.ogg` | `snd_txtmett` | Mettapaw (robô caixa com ring-light) | Sintetizador eletrônico robótico retro (~50ms). Onda quadrada com modulação em anel (ring modulator) em ~440 Hz. | Onda quadrada processada com ring mod em 50 Hz ou bitcrusher clássico de 4 bits. | **P1** |
| `voice_asgrr.ogg` | `snd_txtasg` | Rei Asgrr (cabro gigante cheio de crachás de con) | Voz pesada, imponente, cavernosa e trágica (~70ms). Onda dente-de-serra bem grave em ~85 Hz com reverberação curta embutida. | Frequência sub-grave (~85-90 Hz), onda rica em harmônicos ímpares, filtro passa-baixas em 400 Hz. | **P1** |
| `voice_uwumie.ogg` | `snd_txttem` | Uwu-mie (gatinha caótica da vila) | Sequência caótica e desafinada de micro-beeps agudos (~30ms), alternando entre notas aleatórias (700 Hz - 900 Hz). | Bfxr: Preset *Laser* invertido, arpeggios aleatórios de onda quadrada agudíssima. | **P2** |
| `voice_moderator.ogg` | (Específico nUndertale) | xX_Moderator_Xx (chefe final gamer na cadeira RGB) | Chiado de microfone de headset gamer barato com compressão excessiva e estática de sala de bate-papo de voz. | Voz humana gravada distorcida com filtro passa-banda telefônico (300 Hz a 3 kHz) e ruído estático de fundo. | **P2** |

---

### 3.3 Início de Encontros & Transições de Batalha

O icônico fluxo de engajamento do Undertale (aviso -> estática -> transição para a arena).

| Arquivo Final | Equivalente Undertale | Onde Toca no Jogo | Descrição Sonora (Timbre, Forma de Onda, ADSR) | Como Criar / Obter | Prio. |
| :--- | :--- | :--- | :--- | :--- | :---: |
| `alert.ogg` | `snd_exclamation` | O "!" surge acima da cabeça de Pepe ao iniciar encontro | Estalo sonoro de impacto agudo e dramático (~150ms). Inspirado em *Metal Gear / EarthBound*. Ataque seco em ~800 Hz seguido de decaimento rápido. | Bfxr: Preset *Hit*, onda dente-de-serra, sweep ascendente curtíssimo, sem sustain. | **P0** |
| `encounter_beep.ogg` | `snd_battlefall` / `snd_bencounter` | Os 3 beeps rápidos em staccato antes da tela piscar para preto | Beep digital puro estilo sonar (~60ms). Onda senoidal ou triangular limpa em ~880 Hz (A5), tocada 3 vezes com intervalo de 80ms. | Onda senoidal com envelope trapezoidal de 50ms, ganho estável. | **P0** |
| `encounter_fall.ogg` | `snd_heartdrop` | A alma cai do topo da tela até a moldura de batalha | Whoosh/deslize tonal descendente (~250ms). Onda triangular com pitch sweep caindo de 600 Hz para 120 Hz, simulando gravidade/queda. | Bfxr: Preset *Laser*, inverter sweep (sweep negativo pronunciado), sem ruído. | **P0** |
| `intro_noise.ogg` | `snd_noise` | Flash branco e abertura da logo na tela de título | Trovão / drone sintético grave e vibrante (~800ms). Ruído branco filtrado com passa-baixas ressonante abrindo e fechando. | Audacity: Gerar Ruído Branco -> Aplicar Low Pass Filter com variação de frequência de corte de 80 Hz para 400 Hz. | **P1** |
| `name_swoosh.ogg` | `snd_swoosh` | Flash branco ao confirmar o nome do jogador no início | Transição aérea mágica e etérea (~1.2s). Vento filtrado com ressonância cristalina ascendente e fade out longo. | Gravar sopro em microfone com filtro de vento e aplicar reverb longo de placa (plate reverb). | **P1** |

---

### 3.4 Combate — Ações do Jogador (Player Combat)

Efeitos correspondentes aos 4 botões mestres de ação: FIGHT, ACT, ITEM e MERCY.

| Arquivo Final | Equivalente Undertale | Onde Toca no Jogo | Descrição Sonora (Timbre, Forma de Onda, ADSR) | Como Criar / Obter | Prio. |
| :--- | :--- | :--- | :--- | :--- | :---: |
| `slash.ogg` | `snd_laz` / `snd_attack` | Animação do golpe cortando o alvo no FIGHT | Ruído cortante estilo lâmina / chicote sintetizado (~180ms). Ruído branco modulado por pitch sweep descendente rápido com ressonância média. | Bfxr: Preset *Laser*, usar Noise como gerador, decay rápido e crunch alto. | **P0** |
| `hitmarker.ogg` | `snd_crit` / `snd_strike` | Acerto perfeito (centro da barra do FIGHT) | **Específico nUndertale:** O som clássico e crocante de "hitmarker" de jogos de tiro FPS contemporâneos (~90ms). Clique percussivo metálico/plástico estalado. | Gravação foley de estalo de interruptor de mouse mecânico ou busca por CC0 FPS hitmarker sound. | **P0** |
| `enemy_hit.ogg` | `snd_damage` | O inimigo recebe o golpe e pisca | Impacto pesado com soco e estalo oco (~200ms). Transiente percussivo com grave forte (~100 Hz) e ruído de trituração nos agudos. | Bfxr: Preset *Explosion*, duração reduzida para 0.2s, frequências graves altas. | **P0** |
| `vaporize.ogg` | `snd_vaporized` / `snd_dust` | O monstro é derrotado e se desfaz em poeira/algodão | Dissolução suave e melancólica (~800ms). Ruído branco filtrado com passa-baixas descendo suavemente até o silêncio, sem graves. | Ruído rosa com decaimento exponencial de 0.8s e filtro passa-baixas deslizando de 2 kHz para 100 Hz. | **P0** |
| `spare_poof.ogg` | `snd_poof` / `snd_spare` | Poupar o inimigo (MERCY -> Spare) | Som de "poof" mágico fofo e gasoso (~300ms). Ar comprimido saindo suavemente com um pequeno sino cintilante no final. | Soprar levemente na ponta dos dedos fechados + Bfxr *Pickup* bem sutil no fim. | **P0** |
| `level_up.ogg` | `snd_levelup` | Subir de LV / LOVE após vitória | Fanfarra triunfante curtíssima (~700ms). Arpeggio clássico de 4 notas em onda quadrada brilhante: C - E - G - C (oitava superior) com delay analógico leve. | Bfxr: Preset *Powerup*, notas em oitavas ascendentes, onda quadrada pura. | **P0** |
| `bonk.ogg` | (Específico nUndertale) | Ação de BONK na Rota Furry (substitui o FIGHT) | Pancada de martelo de borracha ou moringa oca de desenho animado (*BONK!*, ~250ms). Ataque de madeira/borracha ressonante com afinação cômica. | Foley: bater levemente com uma colher de pau em uma tigela oca ou melancia; ou sample de efeito Sonivox bonk. | **P2** |

---

### 3.5 Combate — Bullet Hell, Projéteis e Dano à Alma

A dinâmica sonora quando a alma vermelha de Pepe entra no quadro de desvio contra os ataques dos monstros.

| Arquivo Final | Equivalente Undertale | Onde Toca no Jogo | Descrição Sonora (Timbre, Forma de Onda, ADSR) | Como Criar / Obter | Prio. |
| :--- | :--- | :--- | :--- | :--- | :---: |
| `hurt.ogg` | `snd_hurtheart` | A alma de Pepe é atingida por um projétil | Estalo agudo e doloroso de impacto (~120ms). Onda quadrada com pitch sweep rápido de 300 Hz para 60 Hz combinado com ruído. Transmite vulnerabilidade. | Bfxr: Preset *Hit/Hurt*, misturar 70% quadrada e 30% ruído, decay seco de 0.1s. | **P0** |
| `bullet_spawn.ogg` | `snd_ding` / `snd_sparkle` | Projéteis mágicos surgem na arena | Plink cristalino agudo e sutil (~80ms). Onda senoidal ou triangular em ~1500 Hz com attack imediato e decay super rápido. | Bfxr: Preset *Blip*, freq 1500 Hz, decay 0.08s, onda senoidal. | **P0** |
| `spear_throw.ogg` | `snd_spear` / `snd_arrow` | Arpão da Sharkdyne ou flechas sendo arremessadas | Zunido metálico rápido cortando o ar (~150ms). Silvo em alta frequência com sweep descendente de ar comprimido. | Bfxr: Preset *Laser*, onda dente-de-serra, sweep rápido, filtro passa-altas ligado. | **P1** |
| `spear_aim.ogg` | `snd_spearappear` | Mira luminosa avisando de onde virá o arpão | Zumbido elétrico crescente de alerta (~200ms). Pitch subindo de 400 Hz para 1200 Hz com vibrato rápido. | ChipTone: sweep ascendente com LFO modulando pitch a 25 Hz. | **P1** |
| `shield_deflect.ogg` | `snd_arrowreflect` | Escudo da alma verde bloqueando um ataque | Estalo metálico firme e protetor (*Ting!*, ~150ms). Impacto de metal sólido temperado com decay limpo e brilhante. | Foley: bater levemente uma lâmina de tesoura ou colher de ferro contra frigideira. | **P1** |
| `pew_shot.ogg` | `snd_yellow_shot` / `snd_pew` | Disparo da alma amarela contra projéteis | Tiro retrô de nave arcade estilo *Galaga/Space Invaders* (~120ms). Onda quadrada com sweep de 900 Hz para 200 Hz. | Bfxr: Preset *Laser/Shoot*, padrão clássico de chiptune shmup. | **P1** |
| `laser_beam.ogg` | `snd_laser` | Feixe de laser contínuo de Mettapaw ou Gaster Blaster | Ruído eletromagnético pulsante e agressivo (~600ms). Mistura de zumbido de 60 Hz (hum elétrico de tomada) com ruído branco saturado. | Audacity: Gerar tom 60 Hz dente-de-serra + Ruído Branco -> Aplicar Distorção Waveform. | **P1** |
| `explosion.ogg` | `snd_bomb` / `snd_explode` | Bombas de confete ou explosões de itens | Estrondo clássico 8-bit com impacto encorpado (~600ms). Ruído branco com volume decaindo exponencialmente e ressonância grave. | Bfxr: Preset *Explosion*, aumentar Bass Cutoff e tempo de decay para 0.5s. | **P1** |
| `bite.ogg` | `snd_bite` | Ataques dos cães de Snowdinha ou mordida de Barky | Mordida de dentes estalando no ar (*Chomp!*, ~100ms). Dois estalos orgânicos secos e seguidos de impacto de mandíbula. | Foley: fechar duas peças de madeira leve rapidamente ou estalar dentes perto do mic. | **P1** |

---

### 3.6 Mecânicas de Alma & Física da Arena

Sons associados à mudança de gravidade e mecânica da alma de Pepe dentro do BattleBox.

| Arquivo Final | Equivalente Undertale | Onde Toca no Jogo | Descrição Sonora (Timbre, Forma de Onda, ADSR) | Como Criar / Obter | Prio. |
| :--- | :--- | :--- | :--- | :--- | :---: |
| `soul_mode.ogg` | `snd_modechange` / `snd_bell` | Alma mudando de cor (Azul, Verde, Amarela, Roxa) | Sino de sintetizador etéreo com modulação de fase (~300ms). Acorde mágico de 2 notas com brilho aquático. | ChipTone: Preset *Powerup*, onda senoidal com flange e chorus sutil. | **P1** |
| `soul_jump.ogg` | `snd_jump` | Pulo da alma sob gravidade azul | Salto elástico de plataforma 8-bit (~120ms). Onda senoidal ou triangular com sweep rápido subindo de 200 Hz para 550 Hz. | Bfxr: Preset *Jump*, diminuir duração para 0.12s, onda triangular para soar macio. | **P1** |
| `soul_slam.ogg` | `snd_heavyimpact` | Alma sendo arremessada na parede pelo Sans | Pancada violenta e opaca contra superfície dura (~250ms). Grave surdo (~70 Hz) acompanhado de estalo seco de impacto. | Bfxr: Preset *Hit*, muito grave, sweep descendente violento, decay curto. | **P1** |

---

### 3.7 Morte & Fim de Jogo (Game Over / Furry Over)

Undertale trata o Game Over com a destruição trágica da determinação; nUndertale divide em duas vertentes: a fratura gamer clássica e a transformação em pelúcia no FURRY OVER.

| Arquivo Final | Equivalente Undertale | Onde Toca no Jogo | Descrição Sonora (Timbre, Forma de Onda, ADSR) | Como Criar / Obter | Prio. |
| :--- | :--- | :--- | :--- | :--- | :---: |
| `soul_crack.ogg` | `snd_break1` | O coração de Pepe para no ar e se racha ao meio | Fratura de cristal fino / estalo seco de vidro temperado trincando (~180ms). Sem graves, transiente de alta frequência pontiagudo. | Foley: quebrar palito de dente ou estalar casca de ovo seca perto do microfone com boost em 4 kHz. | **P0** |
| `soul_shatter.ogg` | `snd_break2` | Os pedaços do coração explodem e caem da tela | Estilhaçamento de cristal fino se espalhando (~500ms). Múltiplos micro-estalos cintilantes em cascata, estilo taça de cristal quebrando. | Foley: soltar pequenos cacos de vidro/acrílico sobre superfície de mármore com reverb suave. | **P0** |
| `pomf.ogg` | (Específico nUndertale) | FURRY OVER (HP vira FUR 100%) ou aumento de estágio FUR | Queda de um bicho de pelúcia fofo e gordo no carpete macio (*Pomf!*, ~300ms). Impacto de tecido fofo, sem nenhuma dureza, abafado e reconfortante. | Foley: soltar um travesseiro macio ou casaco pesado fofo em cima da cama ou tapete. | **P0** |
| `tape_rip.ogg` | (Específico nUndertale) | Ao recusar a transformação no Espelho (FLF-002) | Som agressivo de fita adesiva larga / silver tape sendo arrancada violentamente de uma superfície lisa (*RIIIIIIP*, ~400ms). | Foley real: puxar silver tape ou fita crepe grossa rapidamente perto do microfone. | **P2** |

---

### 3.8 Overworld, Cenário & Quebra-Cabeças (Puzzles)

Sons de exploração ambiental, passos, interações físicas e engrenagens das Ruínas, Snowdinha, Wetfall e Hotland.

| Arquivo Final | Equivalente Undertale | Onde Toca no Jogo | Descrição Sonora (Timbre, Forma de Onda, ADSR) | Como Criar / Obter | Prio. |
| :--- | :--- | :--- | :--- | :--- | :---: |
| `step_crunch.ogg` | `snd_step` (neve) | Caminhar sobre o chão das Ruínas repleto de salgadinhos | **Específico nUndertale:** Passos triturando Doritos e salgadinhos crocantes sob o piso (~80ms por passo). Estalos secos de milho torrado quebrando. | Foley: pisar ou amassar sacos de salgadinho/tortilhas reais com as mãos num prato. | **P0** |
| `door_open.ogg` | `snd_door` | Portas de madeira ou pedra se abrindo nos quartos e passagens | Ranger clássico de dobradiça de madeira/ferro pesada se abrindo devagar (~450ms). | Gravar porta de armário antigo rangendo devagar, aplicando pitch shift para encorpar. | **P1** |
| `switch_click.ogg` | `snd_switch` | Alavancas, botões de chão e interruptores RGB de puzzle | Clique mecânico táctil industrial bem definido (~70ms). Estalo duplo (*ka-click*) de relé ou chave de energia. | Foley: acionar interruptor de filtro de linha ou disjuntor de energia. | **P0** |
| `spikes.ogg` | `snd_spikes` | Espinhos de puzzle ou barreiras de cabos subindo/descendo | Engrenagem mecânica deslizando dentes de metal/pedra para dentro do piso (~250ms). | Bfxr: Preset *Explosion* muito curto com pitch alto e filtro passa-banda. | **P0** |
| `rock_push.ogg` | `snd_pushrock` | Empurrar pedras de puzzle pelo piso de pedra | Atrito áspero contínuo de rocha raspando sobre lajotas (~400ms). Raspagem mineral média-grave. | Foley: arrastar tijolo ou pedra áspera sobre piso de concreto limpo. | **P1** |
| `zap.ogg` | (Específico nUndertale) | Encostar nos cabos descascados de puzzle nas Ruínas | Estalo de choque elétrico estático / curto-circuito (*BZZZZT-ZAP!*, ~200ms). Onda quadrada distorcida com faísca metálica. | Bfxr: Preset *Laser*, misturar ruído e distorção com decay de 0.2s. | **P0** |
| `fall_long.ogg` | `snd_fall` | Queda inicial de Pepe pela cratera do Monte Dew | Assobio longo e cômico de queda livre com pitch caindo por 2.5 segundos, terminando em silêncio. | Onda senoidal pura em slide descendente contínuo de 1200 Hz para 80 Hz. | **P0** |
| `phone_ring.ogg` | `snd_phone_ring` | Chamadas recebidas no celular flip retrô | Toque de monofone analógico clássico dos anos 90 / telefone de linha (*Trrrriiiim*, 2 pulsos de 400ms). | Dois tons combinados de 440 Hz + 480 Hz (padrão bell americano) modulados por pulso. | **P1** |
| `phone_buzz.ogg` | (Específico nUndertale) | Celular de Pepe vibrando com mensagens do Discord | Vibração dupla de smartphone moderno sobre madeira (*Bzz-bzz*, ~250ms cada pulso). Subgrave abafado e percussivo. | Foley: celular real gravado vibrando em cima de mesa oca de madeira. | **P1** |
| `water_splash.ogg` | `snd_splash` | Passos nas poças de Wetfall ou quedas d'água | Espirro límpido de água limpa (~200ms). Gotículas percussivas com decaimento suave. | Foley: bater a mão levemente em uma bacia com água rasa. | **P1** |

---

### 3.9 Eventos Cômicos, Bosses & Paródia nUndertale (Lore §27.2)

O repertório característico de humor e situações inusitadas de *nUndertale*.

| Arquivo Final | Equivalente Undertale | Momento no Jogo | Descrição Sonora (Timbre, Textura, Duração) | Como Criar / Obter | Prio. |
| :--- | :--- | :--- | :--- | :--- | :---: |
| `zipper.ogg` | (Específico nUndertale) | **Cringe Check:** Pepe verifica as costas da fursuit dos monstros | Zíper metálico/plástico abrindo e fechando com velocidade (*Zzzzzp!*, ~300ms). Som característico de fecho éclair de roupa. | Foley: abrir e fechar o zíper de uma jaqueta ou mochila bem perto do mic. | **P0** |
| `plush_squeak.ogg` | (Específico nUndertale) | Ações de Abraço (Hug / Accept Hug) contra inimigos | Apito estridente e agudo de brinquedo de pelúcia/borracha de cachorro (*Squeak-squik!*, ~180ms). | Foley real: apertar brinquedo de borracha para cães ou patinho de borracha. | **P0** |
| `howl.ogg` | (Específico nUndertale) | Entrada dramática e ataque sônico de Barky | Uivo desafinado e adolescente de lobo emo (*AWOOOOO*, ~1.5s). Começa confiante e desafina comicamente no meio, falhando a voz. | Gravar voz humana imitando uivo de lobo adolescente e falhando intencionalmente com pitch bend. | **P0** |
| `glitter_spray.ogg` | (Específico nUndertale) | Furiell espirrando spray de desodorante para afastar Floofy | Aerossol sibilando sob pressão (*Pssssssht*) acompanhado de um cintilar de sininhos mágicos agudos no ar (~800ms). | Gravar desodorante spray real + overlay de arpeggio de sinos mágicos (chimes). | **P0** |
| `tab_close.ogg` | (Específico nUndertale) | Furiell e Alphys fechando abas suspeitas ao serem flagradas | Sequência frenética de 4 cliques ultra-rápidos de mouse mecânico (*clic-clic-clic-clic*, ~250ms). | Clicar com mouse gamer 4 vezes em altíssima velocidade. | **P1** |
| `airhorn.ogg` | (Específico nUndertale) | Aparição de Senpai Sans e piadas gamer MLG | A clássica corneta de estádio / buzina de ar tripla estridente e distorcida dos memes MLG 2014 (~1.0s). | Sintetizador de buzina de ar com onda dente-de-serra saturada em C5 - C5 - C5. | **P1** |
| `modem.ogg` | (Específico nUndertale) | Ponto de Save na Sala dos Racks em Hotland | Trecho do ruído característico de conexão dial-up de modem 56k (o chiado digital estridente clássico, ~1.8s). | Amostra livre de domínio público de handshake dial-up V.90. | **P1** |
| `notify.ogg` | `snd_bell` | Notificação de mensagem nova da guilda / ding de spare | Ding de notificação moderno de aplicativo de mensagens / Discord (~250ms). Duas notas suaves em marimba sintética. | Marimba eletrônica: G5 -> C6 com decay macio. | **P0** |
| `jail_door.ogg` | (Específico nUndertale) | Animação de trancar na cela rosa da "Horny Jail" | Batida pesada de grade de ferro de prisão se fechando (*CLANG-SHUT!*, ~600ms) com ressonância oca de metal. | Foley: bater portão de ferro ou tampa de bueiro metálica. | **P2** |
| `user_left.ogg` | (Específico nUndertale) | Quando um monstro gamer dá rage-quit na batalha | Voz robótica computadorizada feminina ou masculina dizendo com voz seca: *"User disconnected"* ou *"User left your channel"* (~900ms). | Gerar via sintetizador TTS vintage (Windows Sam / Mac Whisper) com filtro telefônico. | **P2** |
| `bottle_bounce.ogg` | (Específico nUndertale) | Ataque de garrafas arremessadas pelo Bottle Guy | Garrafa plástica de Mountain Dew de 2 litros vazia caindo e quicando no chão (*Thump-boing*, ~300ms). | Foley real: soltar garrafa pet de 2L vazia em piso de madeira. | **P2** |
| `crowd_cheer.ogg` | `snd_applause` | Auditório aplaudindo no programa de quiz de Mettapaw | Palmas empolgadas de plateia de auditório de TV com assobios curtos (~2.0s). | Gravação de auditório com filtro de rádio retrô. | **P1** |
| `badumtss.ogg` | `snd_rimshot` | Piada ruim de Sans ou trocadilho de monstro | O clássico rimshot de comédia stand-up: dois toques de caixa de bateria e um choque de prato (*Ba-dum, tssss!*, ~700ms). | Gravação de bateria acústica com prato splash curto. | **P1** |
| `ribbit.ogg` | (Específico nUndertale) | Easter egg de coaxar de sapo (Pepe) e modo da alma sapo | Coaxar orgânico, grave e gutural de sapo de brejo (*Ribbb-it*, ~400ms). | Gravação de sapo-boi ou foley esfregando balão de látex úmido. | **P1** |

---

## 4. Totais e Matriz de Priorização para Produção

Para organizar sua busca e geração de efeitos sem sobrecarregar a produção, divida a esteira em três pacotes claros:

```
[ P0: Ruínas & Slice Jogável ] (24 Efeitos)
   ├── UI & Navegação: menu_move, menu_select, bell, save_can, flee
   ├── Diálogo Base: voice_default, voice_furiell, voice_barky, voice_floofy
   ├── Encontro: alert, encounter_beep, encounter_fall
   ├── Combate Core: slash, hitmarker, enemy_hit, vaporize, spare_poof, level_up, hurt, bullet_spawn
   ├── Morte/Game Over: soul_crack, soul_shatter, pomf
   ├── Puzzles Ruínas: step_crunch, switch_click, spikes, zap, fall_long
   └── Gags Ruínas: zipper, plush_squeak, howl, glitter_spray, notify

[ P1: Rota Gamer Completa ] (20 Efeitos)
   ├── Vozes: voice_sans, voice_papyrus, voice_sharkdyne, voice_alphys, voice_mettapaw, voice_asgrr
   ├── Mecânicas Combate: spear_throw, spear_aim, shield_deflect, pew_shot, laser_beam, explosion, bite
   ├── Alma Física: soul_mode, soul_jump, soul_slam
   ├── Overworld: door_open, rock_push, phone_ring, phone_buzz, water_splash, intro_noise, name_swoosh
   └── Show & Hotland: airhorn, modem, tab_close, crowd_cheer, badumtss, ribbit

[ P2: Rota Furry, Rota Tóxica & Extras ] (6 Efeitos)
   ├── Combate Alternativo: bonk
   ├── Vozes Especiais: voice_uwumie, voice_moderator
   ├── Fim de Rota: tape_rip
   └── Memes Avançados: jail_door, user_left, bottle_bounce
```

| Prioridade | Escopo de Entrega | Quantidade de SFX | Status de Bloqueio no Código |
| :---: | :--- | :---: | :--- |
| **P0** | **Ruínas Completa (Vertical Slice jogável)** | **24** | **Crítico:** Desbloqueia o gameplay completo das Ruínas sem nenhum áudio temporário ou estranho. |
| **P1** | **Resto da Rota Gamer (Snowdinha, Wetfall, Hotland)** | **20** | **Importante:** Permite jogar toda a jornada até o Rei Asgrr. |
| **P2** | **Rotas Furry, Tóxica e Easter Eggs** | **6** | **Complementar:** Polimento de finalização e piadas específicas de final de jogo. |
| **TOTAL** | **Cobertura 100% Exaustiva** | **50 Efeitos** | Sistema de áudio completo de nUndertale. |

---

## 5. Guia Prático: Onde e Como Conseguir os Efeitos

### 5.1 Síntese Procedural Rápida (100% Original e Gratuita)

Se você mesmo quiser gerar os efeitos sonoros sem preocupação com direitos autorais ou royalties (o áudio gerado pertence integralmente a você):

1. **ChipTone** ([itch.io/chiptone](https://sfbgames.itch.io/chiptone) - Roda no Navegador):
   - A ferramenta mais intuitiva e moderna para estilo pixel art.
   - Ideal para: `menu_move`, `menu_select`, `alert`, `encounter_beep`, `slash`, `hurt`, `bullet_spawn`, `soul_jump`.
2. **Bfxr** ([bfxr.net](https://www.bfxr.net/) - Standalone ou Web):
   - Especialista em emulação de chips NES/GameBoy (Sound Blaster, ondas puras, ruído de 8 bits).
   - Ideal para: `explosion`, `laser_beam`, `pew_shot`, `level_up`, `menu_error`.
3. **Audacity** ([audacityteam.org](https://www.audacityteam.org/) - Grátis e Open Source):
   - Indispensável para: normalizar volume em -3 dB, cortar milissegundos de silêncio no início de cada som (Trim Silence) e exportar em OGG mono 44.1 kHz.

### 5.2 Bancos de Áudio Recomendados (Apenas Licenças Seguras)

Para efeitos orgânicos/foley que a síntese não atinge com naturalidade (como a lata de refrigerante abrindo, zíper, plástico ou tecidos):

1. **Freesound.org** ([freesound.org](https://freesound.org/)):
   - **Regra de ouro:** Filtre sempre por **Creative Commons 0 (CC0)** (Domínio Público).
   - Se usar sons com licença CC-BY (Atribuição), registre imediatamente o link, autor e título no arquivo `assets/LICENSES.md`.
2. **Kenney Game Assets** ([kenney.nl/assets](https://kenney.nl/assets?q=audio)):
   - Pacotes gratuitos sob licença **CC0 (Public Domain)**:
     - *Kenney Interface Sounds* (ótimo para UI e menus).
     - *Kenney Impact Sounds* (ótimo para ataques e dano).
     - *Kenney RPG Audio* (passos, portas, baús).
3. **OpenGameArt.org** ([opengameart.org](https://opengameart.org/)):
   - Filtre por licença CC0 e tags `chiptune sfx` ou `8-bit sound effects`.

---

## 6. Checklist de Validação Antes de Commitar um Novo SFX

Ao salvar um novo arquivo em `assets/sfx/`:

- [ ] O arquivo está nomeado exatamente conforme a tabela (ex: `assets/sfx/save_can.ogg`)?
- [ ] O formato é OGG (ou WAV para cliques curtíssimos)?
- [ ] O arquivo é Mono (1 canal) e está em 44.100 Hz?
- [ ] O silêncio anterior ao primeiro transiente foi cortado (0 ms latency)?
- [ ] O pico máximo está entre -3 dBFS e -2 dBFS (sem estourar nem sumir)?
- [ ] O arquivo substituiu a menção em `assets/PLACEHOLDERS.md` e a licença foi documentada em `assets/LICENSES.md`?
