# nUndertale — Data Assets & Content Schemas

Este diretório contém as definições de conteúdo dirigidas por dados (Data-Driven Design) para o nUndertale.
Nenhum texto de narrativa, atributo de inimigo ou configuração de encontro deve ficar hardcoded no C++.

---

## Estrutura de Diretórios

```
data/
├── enemies/           # Definições de inimigos (stats, sprites, falas, ACTs, drops)
├── encounters/        # Configurações de batalha (grupos de 1-3 inimigos, música, intro)
├── items/             # Catálogo de consumíveis, armas, armaduras e itens-chave
├── rooms/             # Mapas de salas exportados do Tiled (JSON)
├── patterns/          # Configurações e scripts de padrões de balas (bullet hell)
├── shops/             # Inventário e diálogos de vendedores
└── dialogue/          # Arquivos de texto e localização
    ├── en/            # Diálogos em inglês (master)
    └── pt-br/         # Diálogos em português do Brasil
```

---

## Schemas de Referência

### 1. Inimigo (`data/enemies/<id>.json`)
Baseado no Apêndice A do `BACKLOG.md`:
```json
{
  "id": "barky",
  "name": "Barky",
  "hp": 30,
  "atk": 4,
  "def": 2,
  "exp": 20,
  "gold": 10,
  "sprite": "sprites/enemies/barky.json",
  "offset": [0, 0],
  "voice": "barky",
  "check": "enemy.barky.check",
  "acts": [
    {
      "id": "cringe_check",
      "label": "Cringe Check",
      "text": "enemy.barky.act.cringe",
      "effects": { "mercy": 50, "setFlag": "battle.barky.blushed" },
      "response": "enemy.barky.reply.cringe"
    },
    {
      "id": "offer_dew",
      "label": "Offer Dew",
      "text": "enemy.barky.act.dew",
      "requires": { "item": "mtn_dew_warm", "consume": false },
      "effects": { "mercy": 100 },
      "response": "enemy.barky.reply.dew"
    }
  ],
  "spareWhen": { "mercy": 100 },
  "flavor": {
    "default": ["enemy.barky.flavor.1", "enemy.barky.flavor.2"],
    "lowHp": ["enemy.barky.flavor.lowhp"],
    "spareable": ["enemy.barky.flavor.spareable"]
  },
  "dialogue": {
    "default": ["enemy.barky.bubble.1", "enemy.barky.bubble.2"],
    "afterHit": ["enemy.barky.bubble.hit"]
  },
  "patterns": [
    { "id": "uwu_rain", "weight": 2, "params": { "speed": 3, "density": 0.4, "damage": 3, "furry": 6 } },
    { "id": "dew_bounce", "weight": 1, "params": { "cans": 3, "damage": 4, "furry": 4 } }
  ],
  "canFlee": false,
  "music": "music/boss_barky.ogg"
}
```

### 2. Item (`data/items/items.json`)
Baseado em `ITM-001`:
```json
{
  "items": [
    {
      "id": "mtn_dew_warm",
      "name": "Mountain Dew (Warm)",
      "shortName": "MtnDew",
      "type": "consumable",
      "heal": 10,
      "furry": 0,
      "speedBuff": 1.2,
      "info": "Heals 10 HP. Tastes like lukewarm regret and citrus.",
      "useText": "* You drank the Mountain Dew.&* You recovered 10 HP!",
      "buyPrice": 15,
      "sellPrice": 5
    }
  ]
}
```

### 3. Encontro (`data/encounters/<id>.json`)
Baseado em `BFL-004`:
```json
{
  "id": "encounter_barky",
  "enemies": ["barky"],
  "introText": "* Barky blocked your path!",
  "background": "grid",
  "music": "boss_barky",
  "canFlee": false
}
```
