# 2D Colony Simulator Engine

## Key Systems & Architectural Features
* **Data Oriented Design (ECS):** Built using EnTT to maintain contiguous memory and minimize CPU cache misses
* **Low-Level Graphics Engine:** Custom ASCII renderer built using OpenGL and FreeType
* **Spatial Grid Partitioning:** Custom 2D spatial grid manager providing $O(1)$ spatial queries
* **Optimized Entity Pathfinding:** Pathfinding utilizing flow fields and spatial lookups for optimized swarming behavior
* **Dynamic Utility AI:** Multi-criteria evaluation system for entity decision-making

## Tech Stack
* **Language:** C++20 
* **Graphics & Window:** OpenGL
* **Text & Assets:** FreeType Library
* **IDE:** Visual Studio 2022

Swarming Behavior with 5000 Entities in action:
<img width="800" height="450" alt="ezgif-73f7aa321fc71b25" src="https://github.com/user-attachments/assets/a9c3c6e6-7d70-453f-ac91-46baad6806d0" />
