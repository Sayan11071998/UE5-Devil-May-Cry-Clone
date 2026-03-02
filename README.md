# UE5 Devil May Cry Clone

**3D Action Game | Unreal Engine 5 | C++ & Blueprint**

A Devil May Cry–inspired action game focused on **combat feel** — featuring a full combo system with mixed LMB/RMB inputs, soft lock-on, parry, finisher, and rage mode.  
Built primarily in **C++** with a **data-driven architecture**, keeping gameplay logic modular, extensible, and iteration-friendly.

**Tech Stack:**  
Unreal Engine 5 · C++17 · Behavior Trees · Enhanced Input · Animation Blueprints · Data Assets

---

## Development Approach

The player is split into **focused components**, each owning exactly one responsibility.  
All gameplay actors communicate through **`IDMC_CombatInterface`**, which is implemented by the player and all enemy types.

This allows:
- AnimNotifies to be written **once**
- The same animation events to work across **players, melee enemies, and ranged enemies**
- Animation logic to stay fully **decoupled** from gameplay code

![Architecture Overview](Arch.png)

---

## Key Technical Systems

### Combo System & State Management

Combo state is tracked using **three indices**:
- `LightIndex`
- `HeavyIndex`
- `ExtenderIndex`

Mixed combos work in **two phases**:
1. **Starter phase** — determined by how many heavies landed
2. **Extender phase** — triggered on the next heavy input

Resets are **selective**:
- Starting a heavy clears the light index
- Starting a light clears the heavy index

Inputs during an active attack are **buffered**:
- `EBI_LightAttack`
- `EBI_HeavyAttack`
- `EBI_Dodge`

Buffered inputs are consumed when a **save window** opens.  
Save windows are implemented using **`AnimNotifyState` tracks**, placed directly on montage timelines.

![Light Attack](Light Attack.png)

---

### Component-Based Architecture

Instead of a monolithic character class, the player is composed of small, focused components:

- **CombatComponent** — combo logic and attack flow  
- **TargetingComponent** — soft lock-on and rotation control  
- **RageComponent** — rage mode state and VFX sequencing  
- **FinisherComponent** — execution logic  
- **CombatBufferComponent** — input buffering and movement lunges  

The immediate payoff was **debugging clarity**:
- Combo issues → open `CombatComponent`
- Rotation issues mid-attack → open `TargetingComponent`

The same structure keeps enemies clean:
- `EnemyCharacterBase` only contains shared enemy logic
- `EnemyMelee` handles melee-specific behavior
- `EnemyRanged` handles ranged attacks

Each class stays **small, readable, and focused**.

![Heavy Attack](Heavy Attack.png)

---

### Animation Graph & Locomotion

Locomotion and airborne states (Idle, Walk/Run, Jump, DoubleJump) live in the **Katana animation layer**, driven by:
- `bIsFalling`
- `bDoubleJump`
- Speed from `NativeUpdateAnimation`

When a soft target is active:
- Movement switches to **controller-rotation yaw**
- Directional blending uses `BS_DirectionalWalk` for **8-directional strafing**

![Anim Graph](Screenshot 2026-03-02 004130.png)  
![Directional Strafe](Screenshot 2026-03-02 004257.png)  
![Locomotion Debug](Screenshot 2026-03-02 004228.png)

---

### Enemy AI — Behavior Tree

All enemies share a single **`BT_MeleeEnemy`** tree.

- `BTS_UpdateBehavior` runs every **0.2–0.4s**
- Updates a Blackboard enum based on distance:
  - `Chase`
  - `Strafe`
  - `Attack`

Attack thresholds are configured **per enemy in C++**, so the tree contains **no hardcoded values**.

`BTT_Attack` remains **InProgress** until the attack montage ends, preventing the tree from interrupting attacks mid-swing.

---

### Interface System

`IDMC_CombatInterface` defines the contract every combatant implements.

When adding a **ranged enemy**:
- No AnimNotifies were changed
- Only the relevant interface functions were overridden
- Unused methods remained no-ops

This keeps the **animation layer completely decoupled** from gameplay classes.

---

### Data-Driven Design

All combat data lives in **`DMC_ComboDataAsset`**, including:
- Montage references
- Special attack flags
- Damage multipliers
- Rage settings
- Hit reactions

There are **no hardcoded gameplay values**.  
Tuning combat feel or adding new combos is **editor-only** — no recompiles required.

---

## Technical Challenges

### Combo Chaining & Mixed Inputs

An early single-index approach broke as soon as mixed LMB/RMB combos were introduced.  
Splitting logic into **Light, Heavy, and Extender indices** and treating mixed combos as two phases solved the issue.

Incorrect resets caused attacks to resume from the wrong position.  
This was fixed by:
- Resetting the appropriate index when switching attack types
- Firing `Notify_ResetState` at montage end

---

### Too Many States, Too Many Edge Cases

With attacking, dodging, parrying, jumping, finisher, and rage all active, logic checks became scattered and error-prone.

Centralizing everything into **`EDMC_PlayerState`** and validating state at the **entry point of every action** eliminated most edge-case bugs.

---

### Player Drifting Past Enemies

Root motion pushed the player forward slightly each hit, eventually causing them to pass through enemies.

Locking movement felt too stiff.  
Instead, calling `SnapToTarget()` at the start of every `ExecuteAttack()` re-aligns yaw per hit, keeping the player naturally positioned in front of the enemy throughout the combo.

---

## What I Learned

State management is the backbone of any action game — and I learned that the hard way.  
Once `EDMC_PlayerState` was centralized, most edge-case bugs simply disappeared.

The component-based approach dramatically improved debugging speed, and the interface-driven design is something I’ll carry into every future project.  
Writing a system once and having it work everywhere is genuinely satisfying.
