# 🏔️ Trail Safety and Rescue System

A modern C++ terminal application designed for national park trails and mountain rescue operations. This project integrates fundamental **Data Structures (Graph via Adjacency List)** and **Object-Oriented Programming (OOP)** principles to manage registered hikers, trail checkpoints, and emergency rescue units.

Developed as a **3rd Semester B.Tech CSE Project-Based Learning (PBL)** assignment.

---

## 📌 Project Overview (Phase 1 Milestone - 25%)

The primary objective of Phase 1 is establishing a clean, robust, and extensible foundational architecture without premature complexity. 

### Key Highlights
* **Graph-Based Trail Representation:** Checkpoints and trails are modeled as a weighted graph using an **Adjacency List**.
* **Clean OOP Hierarchy:** Encapsulated domain entities with an Abstract Base Class (`User`), derived roles (`Hiker`), and responder units (`RescueTeam`).
* **Modern C++ Memory Management:** Employs `std::unique_ptr` and the **RAII (Resource Acquisition Is Initialization)** paradigm to ensure leak-free polymorphic object ownership.
* **Facade Design Pattern:** A single unified interface (`TrailSafetySystem`) orchestrates user management and graph operations.

---

## 🏛️ System Architecture

### 1. Object-Oriented Design (OOP)
* **Abstraction:** `User` is an **Abstract Base Class (ABC)** with pure virtual methods (`displayDetails() = 0`, `getRole() = 0`), defining a rigid contract for all system participants.
* **Encapsulation:** Internal state (`userId`, `name`, `contactNumber`) is declared `protected:` and exposed only through const-correct getters and validating mutators.
* **Inheritance:** `Hiker` publicly inherits from `User` (`is-a` relationship) and extends it with emergency telemetry (emergency contacts, active checkpoint).
* **Polymorphism:** Runtime dynamic dispatch through virtual functions and smart pointer polymorphism (`std::vector<std::unique_ptr<User>>`).
* **Virtual Destructor:** Ensures clean teardown of derived objects through base class pointers.

### 2. Data Structures (Graph Adjacency List)
* **Underlying Containers:** `std::unordered_map<std::string, std::vector<TrailEdge>>`
* **Why an Adjacency List?** Mountain trail networks are **sparse graphs** ($|E| \ll |V|^2$). An Adjacency List uses $O(V + E)$ memory compared to $O(V^2)$ for an Adjacency Matrix.
* **Why `std::unordered_map`?** Hash-table indexing provides **amortized $O(1)$** vertex lookup and insertion using intuitive string identifiers (e.g., `"Base Camp"`).

---

## 📂 Project Structure

```text
.
├── .gitignore          # Ignores build artifacts (*.exe, *.o, etc.)
├── README.md           # Project documentation and architectural overview
└── main.cpp            # Complete modular C++ implementation
```

---

## 🚀 Getting Started

### Prerequisites
* A modern C++ compiler supporting C++14 or newer (e.g., GCC / MinGW `g++`, Clang, or MSVC).

### Compilation
Open your terminal in the project directory and compile:

```bash
g++ -std=c++14 -Wall -Wextra main.cpp -o main.exe
```

### Execution
Run the compiled executable:

```bash
# Windows
.\main.exe

# Linux / macOS
./main.exe
```

---

## 🖥️ Terminal UI Menu

Upon launch, the application loads sample demo topology (checkpoints & trails) so evaluators can inspect the network immediately:

```text
======================== MAIN MENU ========================
  1. Register a Hiker
  2. Add a Trail to the Map (Graph Edge)
  3. View Current System Status (Users + Graph)
  4. Exit
===========================================================
```

---

## 🗺️ Roadmap
- [x] **Phase 1 (Current - 25%):** Foundational architecture, Graph Adjacency List, OOP hierarchy, and Facade bridge.
- [ ] **Phase 2 (50%):** Shortest path calculation and emergency evacuation routing using **Dijkstra's Algorithm**.
- [ ] **Phase 3 (75%):** File I/O persistence (saving/loading trail maps and user logs from CSV/JSON).
- [ ] **Phase 4 (100%):** Real-time SOS dispatch simulator and distress alert notifications.

---

## 👤 Author
* **Vidarbh Dabral** ([@Vidarbh14](https://github.com/Vidarbh14))
* 2nd-Year B.Tech Computer Science & Engineering
