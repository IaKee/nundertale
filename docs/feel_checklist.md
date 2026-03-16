# nUndertale Game Feel & 1:1 Parity Checklist (QA-015)

Este documento estabelece o checklist de verificação de sensação ("feel"), resposta e timing do nUndertale em comparação lado a lado com Undertale (PC v1.08).

---

## 1. Movimentação e Resposta da Alma (Battle Heart)
- [ ] **Velocidade Padrão:** 120 px/s virtual (4 px por tick a 30 Hz). A movimentação deve responder no primeiro frame sem aceleração/inércia.
- [ ] **Hitbox Exata:** A alma visual tem 16×16 px, mas a caixa de colisão efetiva contra projéteis é estritamente 8×8 px centralizada (`QA-002` no F2 overlay).
- [ ] **Diagonal sem normalização:** A velocidade na diagonal em batalha mantém velocidade consistente nos eixos X e Y.
- [ ] **Buffs e Modificadores:** Itens como Mountain Dew aplicam +35% de velocidade e produzem rastros verdes sem interferir na precisão do hitbox.

---

## 2. Ritmo, Digitação e Controles de Texto
- [ ] **Velocidade de Digitação:** ~30 caracteres por segundo (1 caractere por tick a 30 Hz).
- [ ] **Pausas de Pontuação:** Pausa automática perceptível (3 a 6 frames) após vírgulas, pontos finais, exclamações e interrogações.
- [ ] **Comportamento das Teclas:**
  - **[X / Shift]:** Pula a animação de digitação e exibe a página inteira instantaneamente.
  - **[Z / Enter]:** NÃO pula texto enquanto a digitação estiver ativa; apenas avança para a próxima página após a conclusão.
- [ ] **Vozes e Bips:** Som de fala emitido a cada caractere não-espaço com o pitch correto do personagem.

---

## 3. Transição de Encontro (Encounter Transition)
- [ ] **Balão de Alerta ("!"):** Aparece sobre o Pepe por ~15 frames acompanhado do som de alerta (`snd_b`).
- [ ] **Corte para Preto:** O overworld congela e apaga imediatamente.
- [ ] **Piscar da Alma:** A alma vermelha pisca 3 vezes acompanhada do som `snd_bell` / `snd_noise`.
- [ ] **Glide para a Arena:** A alma desliza suavemente da posição do jogador até o centro da caixa de batalha.

---

## 4. Menus de Batalha e Overworld
- [ ] **Sem Input Lag:** Toda navegação de cursor (cima/baixo/esquerda/direita) responde com áudio imediato e troca de seleção em 0 frames de atraso.
- [ ] **Cursor da Alma:** Posicionado imediatamente à esquerda da opção selecionada.
- [ ] **Layout 1:1 da Arena:** Menu desenhado dentro da caixa (32, 250, 574, 140) com fonte monospace pixel-perfect em escala inteira (2×).

---

## 5. Game Over (Furry Over)
- [ ] **Sequência de Morte:** Alma para no local do dano fatal, quebra em duas metades com som de quebra, estilhaça em 6 partículas.
- [ ] **Tela de Determinação:** Texto datilografado confortando o jogador ("Don't lose hope... You can't give up just yet!").
- [ ] **Retorno ao Save:** Pressionar Z recarrega o `file0.json` no último save point com HP e itens preservados.

---

## 6. Overworld e Persistência de Sala
- [ ] **Velocidade do Pepe:** 180 px/s virtual (6 px por frame).
- [ ] **Caixa de Interação:** Z projeta a interação apenas na direção para onde o Pepe está olhando.
- [ ] **Reatividade do Mundo:** NPCs derrotados/poupados atualizam seus diálogos ou desaparecem (deixando poeira se mortos).
