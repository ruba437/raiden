# AI Agent Guide for Raiden Project

## Project Overview

**Raiden** is a 2D shoot-em-up game built with **PTSD framework** (Practical Tools for Simple Design), a C++17 game engine using SDL2, OpenGL, GLM, and ImGui. The project follows a state-machine architecture for game flow and inheritance-based object system for game entities.

### Key Technologies
- **Framework**: PTSD (custom game engine)
- **Build**: CMake with Ninja support recommended
- **Language**: C++17, must build in Debug mode only
- **Graphics**: SDL2, OpenGL, GLM
- **Asset Path**: `Resources/` (embedded via `RESOURCE_DIR` macro at compile time)

---

## Architecture & Critical Patterns

### 1. **Game State Machine** (main loop design)
**Location**: `src/main.cpp`, `include/App.hpp`

The entire game runs via a state machine with these states:
- `START` → `MENU` → `INTRO` → `UPDATE` (gameplay) → `LEVELEND` → `END`

Each state has a dedicated `Update*()` method (e.g., `UpdateMenu()`, `Update()`). The main loop in `main.cpp` switches states and calls `context->Update()` each frame—**do NOT bypass this context update**, as it handles input, timing, and rendering.

**Pattern**: Add new game states to the `enum class State` and create corresponding handler methods.

### 2. **GameObject Hierarchy** (object-oriented entity system)
**Base class**: `Util::GameObject` (from PTSD, in `PTSD/include/Util/GameObject.hpp`)

All game entities inherit from `GameObject`:
- **Player**, **Bullet**, **Enemy** (base), **Background**, **Item**, **UI elements** (ScoreUI, HpUI, etc.)
- Enemy subclasses use **polymorphism**: `AssaultEnemy`, `SpreadEnemy`, `TankEnemy`, `PhantomEnemy`, `TurretEnemy`, `BossEnemy`

**Critical pattern**: Every GameObject has:
- `m_Transform` (position `glm::vec2`, rotation, scale)
- `m_Drawable` (a `shared_ptr<Util::Image>` or other drawable)
- `m_ZIndex` (for draw order, higher = on top)

Override `Update()` in subclasses; App calls `Update()` in its game loop for all entities.

### 3. **Asset Management & Resource Paths**
**Macro**: `RESOURCE_DIR` (defined at compile time via CMakeLists.txt line 63)

All asset paths use: `RESOURCE_DIR "/Image/..."`, `RESOURCE_DIR "/Audio/..."`, etc.
- Images: `Resources/image/` (backgrounds, enemies, bullets, items)
- Fonts: `Resources/font/`
- Audio: configured but not yet populated

**Constraint**: Only works in Debug build; Release is broken (per README).

### 4. **Enemy Spawn System** (level design data)
**Location**: `include/App.hpp` (struct `EnemySpawnData`), `src/App.cpp`

Levels are defined via:
```cpp
struct EnemySpawnData {
    float spawnTime;       // Frame number when to spawn
    glm::vec2 position;    // Spawn location
    EnemyType type;        // Which enemy class
    bool randomX = false;  // Randomize X position
};
```

A `vector<EnemySpawnData> m_LevelEvents` stores the entire level timeline. The app increments `m_LevelTimer` and spawns enemies when timer matches `spawnTime`. **To add levels**: populate `m_LevelEvents` in `Start()` or a separate function.

### 5. **Collision & Gameplay State**
**Key mechanics**:
- Player position: updated each frame via `Update()` based on input
- Bullet updates: move along velocity + acceleration vectors
- Enemy AI: each subclass overrides `Update(const glm::vec2& playerPos)` with unique behavior
- Collision detection: manual distance checks (not physics-based) in App's `Update()` loop

**Pattern**: Check collisions after all entities update but before rendering. Use `GetPosition()` or `m_Transform.translation` directly.

### 6. **Input Handling**
**Location**: PTSD's `Util::Input` (not directly in this codebase)

Usage pattern in App:
```cpp
Util::Input::IsKeyPressed(Util::Keycode::W)  // Query input each frame
```

Steering is typically read as `(W, A, S, D)` or arrow keys; add new input handling in `App::Update()` switch statement.

---

## Build & Test Workflow

### Build Commands
```bash
# Debug build (REQUIRED; Release is broken)
cmake -DCMAKE_BUILD_TYPE=Debug -B build  # Add -G Ninja for faster builds

# Compile
cmake --build build
# Or with Ninja: ninja -C build
```

**Output**: `cmake-build-debug/REPLACE_WITH_YOUR_PROJECT_NAME.exe` on Windows

### Testing
**Location**: `PTSD/test/` (SimpleTest.cpp, TransformTest.cpp, NotSimpleTest.cpp, Interactive/)

Not integrated into main build; PTSD uses GoogleTest. Add tests for new game mechanics in `PTSD/test/` and link via PTSD's CMakeLists.

---

## Coding Conventions & Gotchas

### 1. **Member Variable Naming**
- Private/protected: `m_CamelCase` (e.g., `m_ImgStraight`, `m_ShootTimer`)
- Methods: `camelCase()` (e.g., `SetDirection()`, `GetPosition()`)

### 2. **Memory Management**
All game entities use `shared_ptr<>` for automatic cleanup. App stores collections:
```cpp
std::vector<std::shared_ptr<Enemy>> m_Enemies;
std::vector<std::shared_ptr<Bullet>> m_Bullets;
```

Remove dead/off-screen entities via `.erase()` with `erase-remove` idiom to avoid iterator invalidation.

### 3. **Chinese Comments Prevalent**
The codebase has extensive Chinese comments (game UI strings, level design notes). Preserve these when editing.

### 4. **Virtual Destructors**
Enemy base class uses `virtual ~Enemy() = default;`. Follow this pattern for all polymorphic classes.

### 5. **GLM Vector Operations**
Positions and velocities are `glm::vec2` (x, y). Direct arithmetic is supported:
```cpp
m_Transform.translation += velocity * speed;
float distance = glm::length(pos1 - pos2);
```

### 6. **Rotation & Angles**
Rotations are in **radians**. Convert via `std::atan2(dy, dx)` for direction angles. Common pattern: `angle - 1.570796f` (≈ π/2) to adjust sprite facing.

---

## File Organization

### Core Application
- `src/main.cpp` – Entry point, state machine loop
- `src/App.cpp` – Game logic (900+ lines), state handlers, enemy spawning
- `include/App.hpp` – App class definition, state enum, level data structures

### Game Entities (all inherit `Util::GameObject`)
- `Player.hpp/.cpp` – Player movement, weapons, invincibility
- `Enemy.hpp` + subclasses – Base enemy class + specialized enemy types
- `Bullet.hpp` – Projectiles (velocity + acceleration)
- `Item.hpp` – Power-ups and pickups
- `Background.hpp`, `Deck.hpp` – Environmental objects
- `Score.hpp` – UI elements (ScoreUI, HpUI, BombUI, IconUI)

### Supporting Files
- `CMakeLists.txt` – Build configuration (RESOURCE_DIR path, dependency linking)
- `files.cmake` – Explicit source/header file listings (no glob patterns)
- `PTSD/` – Vendored game framework (do not edit)

---

## Common Tasks & AI Patterns

### Adding a New Enemy Type
1. Create `include/NewEnemy.hpp` inheriting `Enemy`
2. Override `Update(const glm::vec2& playerPos)` with AI logic
3. Add to `EnemyType enum` and `App::Update()` spawn logic
4. Include in `CMakeLists.txt` target_sources

### Adding a New Game State
1. Add state to `App::State enum`
2. Implement `App::UpdateStateName()`
3. Add case in main loop switch statement
4. Handle transitions to next state

### Debugging Entity Position Issues
Check `m_Transform.translation` directly or use `GetPosition()`. All collision tests use `glm::length(pos1 - pos2)` for distance. Off-screen entities at Y > 300 or Y < -300 should be cleaned up.

### Managing Resources
All paths use `RESOURCE_DIR` macro. Add new asset types:
1. Place in `Resources/subfolder/`
2. Use `RESOURCE_DIR "/subfolder/filename"` in constructors
3. Ensure CMakeLists compile-time `RESOURCE_DIR` matches path

---

## Known Constraints & TODOs

- **Release build broken**: CMakeLists forces Debug (line 62-67)
- **PTSD is pinned to v0.2**: URL-based fetch from GitHub release
- **No physics engine**: All movement is manual transforms + distance checks
- **Minimal error handling**: Crashes on missing assets—ensure all `RESOURCE_DIR` paths exist
- **No dynamic resource loading**: Assets must exist at compile time

---

## Quick Reference: Where to Make Changes

| Task | File(s) |
|------|---------|
| Add/modify game states | `src/main.cpp`, `include/App.hpp`, `src/App.cpp` |
| Add new enemy | Create `.hpp`, add to spawn system in `App::Update()` |
| Tweak player movement speed | `include/Player.hpp` constructor or `App::Update()` |
| Add UI element | Create `.hpp` inheriting `Util::GameObject`, render in `App::Update()` |
| Adjust collision radius | `src/App.cpp` distance checks (~line 400+) |
| New asset type | Add folder in `Resources/`, use `RESOURCE_DIR "/type/..."` in code |
