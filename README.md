# UE5 Devil May Cry Clone

**3D Action Game | Unreal Engine 5 | C++ & Blueprint**

A Devil May Cry–inspired action game focused on **combat feel** — featuring a full combo system with mixed LMB/RMB inputs, soft lock-on, parry, finisher, and rage mode.  
Built primarily in **C++** with a **data-driven architecture**, keeping gameplay logic modular, extensible, and iteration-friendly.

**Tech Stack:**  
`Unreal Engine 5` `C++17` `Behavior Trees` `Enhanced Input` `Animation Blueprints` `Data Assets`

---

## Architecture Overview

The player is split into **focused components**, each owning exactly one responsibility.  
All gameplay actors communicate through **`IDMC_CombatInterface`**, implemented by the player and all enemy types.

This allows:
- AnimNotifies to be written **once**
- The same animation events to work across **players, melee enemies, and ranged enemies**
- Animation logic to stay fully **decoupled** from gameplay code

<img width="6777" height="2840" alt="Image" src="https://github.com/user-attachments/assets/f6a7106e-a694-444e-8f0d-06f07a0719b7" />

---

## Key Technical Systems

### Combo System & State Management

Combo state is tracked using **three indices**:

| Index | Purpose |
|-------|---------|
| `LightIndex` | Tracks light attack chain |
| `HeavyIndex` | Tracks heavy attack chain |
| `ExtenderIndex` | Triggers extender phase |

Mixed combos work in **two phases**:
1. **Starter phase** — determined by how many heavies landed
2. **Extender phase** — triggered on the next heavy input

Resets are **selective** — starting a heavy clears `LightIndex`, and vice versa.

Inputs during an active attack are **buffered** (`EBI_LightAttack`, `EBI_HeavyAttack`, `EBI_Dodge`) and consumed when a **save window** opens. Save windows are placed directly on montage timelines using **`AnimNotifyState` tracks**.

<img width="1358" height="947" alt="Image" src="https://github.com/user-attachments/assets/f5c7f30d-2866-4ce1-b8d2-9a6b39ea3f96" />

<img width="1453" height="985" alt="Image" src="https://github.com/user-attachments/assets/d977778f-0d64-4d2e-9622-9772a6977dd8" />

---

### Component-Based Architecture

Instead of a monolithic character class, the player is composed of small, focused components:

| Component | Responsibility |
|-----------|---------------|
| `CombatComponent` | Combo logic and attack flow |
| `TargetingComponent` | Soft lock-on and rotation control |
| `RageComponent` | Rage mode state and VFX sequencing |
| `FinisherComponent` | Execution logic |
| `CombatBufferComponent` | Input buffering and movement lunges |

The immediate payoff is **debugging clarity** — combo issues go to `CombatComponent`, rotation issues go to `TargetingComponent`.

The same structure keeps enemies clean:
- `EnemyCharacterBase` — shared enemy logic only
- `EnemyMelee` — melee-specific behavior
- `EnemyRanged` — ranged attacks

<img width="1188" height="962" alt="Image" src="https://github.com/user-attachments/assets/1f56c9e2-ac94-4b36-b01f-ee0006235d56" />

---

### Animation Graph & Locomotion

Locomotion and airborne states live in the **Katana animation layer**, driven by `bIsFalling`, `bDoubleJump`, and speed from `NativeUpdateAnimation`.

When a soft target is active, movement switches to **controller-rotation yaw** and directional blending uses `BS_DirectionalWalk` for **8-directional strafing**.

<img width="1430" height="976" alt="Image" src="https://github.com/user-attachments/assets/0cfb434f-6974-460b-94ab-a09c58ea7566" />

<img width="1453" height="977" alt="Image" src="https://github.com/user-attachments/assets/e4243940-4f2b-4c17-b08f-984522f4e839" />

---

### Enemy AI — Behavior Tree

All enemies share a single **`BT_MeleeEnemy`** tree.

- `BTS_UpdateBehavior` ticks every **0.2–0.4s**, updating a Blackboard enum based on distance: `Chase` → `Strafe` → `Attack`
- Attack thresholds are **configured per enemy in C++** — no hardcoded values in the tree
- `BTT_Attack` stays **InProgress** until the montage ends, preventing mid-swing interruptions

<img width="1507" height="948" alt="Image" src="https://github.com/user-attachments/assets/e89f98fe-0813-4de9-ae70-57320e4a8d0d" />

---

### Interface System

`IDMC_CombatInterface` defines the contract every combatant implements.

When adding a **ranged enemy**, no AnimNotifies were changed — only the relevant interface functions were overridden, and unused methods remained no-ops. The **animation layer stays completely decoupled** from gameplay classes.

---

### Data-Driven Design

All combat data lives in **`DMC_ComboDataAsset`**:
- Montage references
- Special attack flags
- Damage multipliers
- Rage settings
- Hit reactions

There are **no hardcoded gameplay values**. Tuning combat feel or adding new combos is **editor-only** — no recompiles required.

---

## Technical Challenges

### Combo Chaining & Mixed Inputs
An early single-index approach broke as soon as mixed LMB/RMB combos were introduced. Splitting into **Light, Heavy, and Extender indices** and treating mixed combos as two phases solved it. Incorrect resets were fixed by resetting the appropriate index on attack-type switch and firing `Notify_ResetState` at montage end.

### State Explosion
With attacking, dodging, parrying, jumping, finisher, and rage all active simultaneously, logic checks became scattered. Centralizing everything into **`EDMC_PlayerState`** and validating state at the **entry point of every action** eliminated most edge-case bugs.

### Player Drifting Through Enemies
Root motion pushed the player forward each hit, eventually causing pass-throughs. Locking movement felt too stiff — instead, calling `SnapToTarget()` at the start of every `ExecuteAttack()` re-aligns yaw per hit, keeping the player naturally positioned throughout the combo.

---

## What I Learned

> State management is the backbone of any action game — and I learned that the hard way.

Once `EDMC_PlayerState` was centralized, most edge-case bugs simply disappeared. The component-based approach dramatically improved debugging speed, and the interface-driven design is something I'll carry into every future project. Writing a system once and having it work everywhere is genuinely satisfying.
---
[![Watch the video](https://img.youtube.com/vi/n6X03u8bapQ/maxresdefault.jpg)](https://youtu.be/n6X03u8bapQ)
### [Gameplay Video](https://youtu.be/n6X03u8bapQ)
---

<img width="1920" height="1080" alt="Image" src="https://github.com/user-attachments/assets/21f576c3-6874-437d-a528-48c615f2cb12" />

<img width="1920" height="1080" alt="Image" src="https://github.com/user-attachments/assets/46a4ebd7-2fb8-4d2e-b52f-7b5636b5fc69" />

<img width="1920" height="1080" alt="Image" src="https://github.com/user-attachments/assets/c0fc473b-f341-434d-b3de-e9bda83eb6cf" />

<img width="1920" height="1080" alt="Image" src="https://github.com/user-attachments/assets/1a15489c-ded0-4f03-a0f1-67f22f032d52" />

<img width="1920" height="1080" alt="Image" src="https://github.com/user-attachments/assets/00b03f67-875e-4940-930d-4a0537a1e8cf" />

<img width="1920" height="1080" alt="Image" src="https://github.com/user-attachments/assets/c4b6f88e-0fb0-4404-9f38-d12e40fb057e" />

<img width="1920" height="1080" alt="Image" src="https://github.com/user-attachments/assets/921b25b3-df39-4cac-ad67-f6bb6f125b66" />
