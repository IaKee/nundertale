# nUndertale 🐸

> An indie personal RPG comedy project, paying comedic tribute to Toby Fox's classic *Undertale*.

---

## 🌟 Overview

**nUndertale** is a comedic parody inspired by *Undertale*. In the game, the player assumes the role of **Pepe the Fallen Gamer**, who plummets into the forgotten underworld of **Mt. Dew** — a subterranean realm inhabited by eccentric internet tribes (Furries, Weeaboos, and Roleplayers).

Unlike the original game, losing all your HP in *nUndertale* does not simply destroy your soul: Pepe succumbs to the surrounding "cringe" and undergoes a gradual metamorphosis into a furry! Your objective is to explore the Ruins, solve ridiculous puzzles, interact with eccentric characters, dodge meme bullets in retro bullet-hell encounters, and find your way back to your gaming PC.

---

## 🕹️ Playable Minimum Level Features

- **Disclaimer Screen:** Nostalgic opening screen with smooth fade-in, authentic fonts (`monster_friend.otf`, `gomarice_shmup_in_the_zone.ttf`), and clean personal indie project attribution (press Z or Enter to skip).
- **Intro Lore Sequence:** 1:1 Undertale style slideshow across 10 cards (`lore1.bmp` through `lore9.bmp` + animated fall sequence `lore10`), chiptune soundtrack (*Once Upon A Time Remix*), and typewriter sound effects.
- **Main Title Screen:** Authentic Undertale title font (`monster_friend_fore.otf`), floating logo with red `#` emblem, random quotes, flashing `[Press Z or Enter]`, and menu options with frog-soul selector.
- **Instruction Manual:** Explains controls and the iconic rule: *"When your HP is 0, you become a furry."*
- **Character Naming ("Name the fallen gamer"):**
  - Interactive 4x7 character matrix with subtle authentic pixel-jitter.
  - Special buttons: `[PRO]`, `[GAMER]`, `[Backspace]`, `[Done]`.
  - Dynamic Easter Eggs with unique audio/visual reactions for: `CHAD` (golden border), `PEPE`, `SKYRIM`, `SANS`, `FRISK`, `CHARA`, `FLOWEY`, `TORIEL`, `ASGORE`, `RUIVA`, `LOIRA`, `OTOME`, `GOTICA`, `MORENA`, repeated letters, and spamming limits.
  - Confirmation screen: *"Are you sure about that?"* with animated zooming name, PI angle jitter, and YES/NO choices.
- **Overworld Exploration (Playable Test Level):**
  - Full 4-directional walking animations for Pepe using the original spritesheet (`pepe_char.bmp`) with magenta color-key transparency.
  - Virtual 640x480 retro canvas with pixel-perfect letterboxing on modern high-resolution displays.
  - Sparkling animated Save Point: heals HP to 20/20, plays sound, and displays Determination text.
  - Interactive signpost and Barky the Alpha Wolf NPC with typewriter dialogue box and sound bleeps.
  - Northern exit doorway that triggers the battle encounter!
- **Battle Encounter System:**
  - Turn-based parody combat against **Barky the Alpha Wolf**.
  - Authentic Undertale menu actions: `[ FIGHT ]`, `[ ACT ]`, `[ ITEM ]`, `[ MERCY ]`.
  - Attack timing bar minigame for critical hits.
  - Real-time bullet-hell dodge box: control the green soul to dodge cascading "UWU" bullets and bouncing Mountain Dew soda cans.
  - Furry-meter and HP bar tracking.

---

## 🏗️ Software Architecture & SOLID Principles

The project was migrated from a legacy procedural singleton structure to **Modern C++20** using the hardware-accelerated **Raylib** library:

- **S (Single Responsibility Principle):** Each subsystem (`AudioService`, `ResourceService`, `InputService`, `SceneManager`, `DialogBox`, `Player`, `BattleSystem`) has one well-defined responsibility.
- **O (Open/Closed Principle):** The `IScene` and `IInteractable` interfaces allow adding new rooms, scenes, and mini-games without modifying the engine core loop.
- **L (Liskov Substitution Principle):** All scenes (`DisclaimerScene`, `LoreScene`, `TitleScene`, `NameSelectScene`, `OverworldScene`, `BattleScene`) are interchangeable and managed uniformly by `SceneManager`.
- **I (Interface Segregation Principle):** Lightweight, focused abstract contracts (`IScene`, `IAudioService`, `IResourceService`, `IInputService`, `IInteractable`).
- **D (Dependency Inversion Principle):** High-level scenes depend upon abstractions provided by `GameContext`, completely eliminating brittle global state and singletons.

---

## 🎮 Controls

| Key | Action |
| :--- | :--- |
| **Arrow keys / W, A, S, D** | Move Pepe / Menu Navigation / Dodge in Battle Box |
| **Z / Enter / Space** | Confirm / Interact / Advance Dialogue / Attack |
| **X / Shift** | Cancel / Run / Backspace / Fast-forward text |
| **C / Ctrl** | Menu / Inventory |
| **B** | Quick battle encounter shortcut (in overworld) |
| **Esc** | Return to Previous Menu / Exit |

---

## 🛠️ How to Build and Run

The project includes a portable GCC/G++ compiler and Raylib in the `vendor/` directory, allowing clean native compilation without external dependencies or DLL hell:

### Quick Build on Windows (PowerShell / Command Prompt):
```powershell
.\build.bat
```

### Run Game:
```powershell
.\nundertale_modern.exe
```

---

## 📖 Script & Game Design Document
For full lore, character dialogue scripts, and design documentation, see [ROTEIRO.md](file:///c:/Users/delux/OneDrive/Documentos/Programas/C/nundertale/ROTEIRO.md).
