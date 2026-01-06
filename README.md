# Pac-Man Style Maze Game (C++ / Qt) — OOP + Pathfinding AI

A multi-level arcade-style maze game built in **C++** using the **Qt framework**.  
The project focuses on clean object-oriented design and enemy AI movement using **heuristic pathfinding (A\*)** on a grid-based map.

## Features
- **5 levels** with increasing difficulty
- Real-time **Qt GUI** (HUD, animations, sound effects)
- Modular **OOP architecture** (player, enemies, items, level manager)
- Enemy AI with **A\* pathfinding** (Manhattan heuristic) for chase behavior
- Multiple enemy states (e.g., normal / panic / return-to-spawn) with state-dependent movement logic
- Collision handling, scoring, and life/health management

## Tech Stack
- **Language:** C++
- **Framework:** Qt (Widgets / QGraphicsScene)
- **Algorithms:** A* pathfinding (grid), state-driven AI logic
- **Assets:** Qt Resource System (`resources.qrc`)

## Project Structure
- `source/` — main C++ source code (game logic, entities, pathfinding)
- `game_objects/` — sprites/audio assets (via Qt resources)
- `resources.qrc` — Qt resource mapping
- `cleangame.pro` — Qt project file
- `mainwindow.ui` — UI layout (Qt Designer)

## How to Run
1. Open `cleangame.pro` in **Qt Creator**
2. Configure a kit (Desktop Qt)
3. Build and Run


