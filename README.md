# 2D Colony Simulator Engine

## Key Systems & Architectural Features
* **Data Oriented Design (ECS):** Built using EnTT to maintain contiguous memory and minimize CPU cache misses
* **Low-Level Graphics Engine:** Custom ASCII renderer built using OpenGL and FreeType
* **Spatial Grid Partitioning:** Custom 2D spatial grid manager providing $O(1)$ spatial queries
* **Dynamic Utility AI:** Multi-criteria evaluation system for entity decision-making

## Tech Stack
* **Language:** C++20 
* **Graphics & Window:** OpenGL
* **Text & Assets:** FreeType Library
* **IDE:** Visual Studio 2022
