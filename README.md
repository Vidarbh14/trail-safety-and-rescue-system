# 🏔️ Trail Safety and Rescue System

A C++ terminal application for offroad trails and mountain rescue operations. It models a trail network as a **weighted graph (adjacency list)** and uses **Dijkstra's algorithm** to find shortest routes and dispatch the nearest available rescue team to a hiker in distress.

Developed as a **3rd Semester B.Tech CSE Project-Based Learning (PBL)** assignment.

---

## Project Status

| Phase | Target | Status |
|---|---|---|
| **Phase 1** | Graph storage, OOP hierarchy, facade | ✅ Complete |
| **Phase 2** | Dijkstra shortest path + SOS rescue dispatch | ✅ Complete |
| **Phase 3** | File I/O persistence (Save & Load) + Web JSON Export | ✅ Complete |
| **Phase 4** | Multi-hiker alerts, trail closures, risk scoring | ✅ Complete |
| **Phase 5** | Automatic Location Tracking & Telemetry Telemetry | ✅ Complete |
| **Phase 6** | Modularization into `.hpp`/`.cpp` per team member | ✅ Complete |
| **Phase 7** | Interactive Web Dashboard (`web/index.html`) | ✅ Complete |

---

## 🏛️ Architecture

### 1. Object-Oriented Design (Owner: Arnav Negi)
* **Abstraction** — `User` is an Abstract Base Class with pure virtual methods (`displayDetails()`, `getRole()`).
* **Encapsulation** — State is `private`/`protected` and accessed via const-correct getters and validating mutators.
* **Inheritance** — `Hiker` publicly inherits `User` and adds checkpoint telemetry, trek state, and travel history.
* **Polymorphism** — Runtime dispatch through base class pointers (`std::vector<std::unique_ptr<User>>`).
* **RAII** — `std::unique_ptr` ensures leak-free ownership; virtual destructor guarantees clean derived teardown.

### 2. Data Structure — Graph as Adjacency List (Owner: Vidarbh Dabral)
```cpp
std::unordered_map<std::string, std::vector<TrailEdge>>
```
* **Why an adjacency list?** Trail networks are **sparse** (|E| ≪ |V|²). Adjacency list costs **O(V + E)** space versus **O(V²)** for an adjacency matrix.
* **Why `unordered_map`?** Hash indexing provides amortised **O(1)** lookup by readable checkpoint name.

### 3. Algorithm — Dijkstra's Shortest Path (Owner: Vidarbh Dabral)
* **Min-heap**: `std::priority_queue<DistNode, vector<DistNode>, greater<DistNode>>` always extracts the closest unsettled checkpoint.
* **Complexity**: **O((V + E) log V)**.
* **Dynamic Hazard Bypass**: Automatically avoids `CLOSED` trail edges and warns about `CAUTION` segments.

### 4. System Facade, Tracker & File I/O (Owner: Shorya Negi)
* Coordinates graph and entities through `TrailSafetySystem`.
* Implements automated checkpoint tracking and dynamic SOS dispatch to the hiker's latest verified location.
* Persists data to `trail_safety_data.txt` and exports web-ready JSON to `trail_network.json`.

### 5. UI, Validation & Progress Bar (Owner: Rajat Pant)
* Interactive 17-option CLI menu with stream cleansing and numeric validation.
* Animated ASCII progress bar for live simulation.

---

## 📂 Project Structure

```text
.
├── include/
│   ├── Common.hpp            # Cross-platform time, sleep & infinity helpers
│   ├── User.hpp              # Abstract base class & CheckpointCrossing (Arnav Negi)
│   ├── Hiker.hpp             # Hiker entity & telemetry history (Arnav Negi)
│   ├── RescueTeam.hpp        # Rescue unit entity & dispatch status (Arnav Negi)
│   ├── UserLinkedList.hpp    # Custom Singly Linked List Data Structure (Arnav Negi)
│   ├── TrailGraph.hpp        # Adjacency list & Dijkstra header (Vidarbh Dabral)
│   ├── TrailSafetySystem.hpp # Facade system & auto-tracker (Shorya Negi)
│   └── UI.hpp                # Progress bar & terminal menu (Rajat Pant)
├── src/
│   ├── TrailGraph.cpp        # Graph algorithms & hazard bypass (Vidarbh Dabral)
│   ├── TrailSafetySystem.cpp # System implementation, I/O & SOS (Shorya Negi)
│   └── UI.cpp                # UI functions & validation (Rajat Pant)
├── web/
│   └── index.html            # Interactive visual web dashboard (HTML5 Canvas + JS)
├── main.cpp                  # Modular application entry point
├── build.bat                 # 1-click Windows build script
├── run_demo.bat              # 1-click automated viva test demo
├── trail_network.json        # Web dashboard network data
└── trail_safety_data.txt     # System state persistence file
```

---

## 🚀 Getting Started

### Prerequisites
A C++ compiler supporting C++14 or newer (GCC/MinGW `g++`, Clang, or MSVC).

### Compile
```bash
# Using build script (Windows)
.\build.bat

# Or manual compilation with g++:
g++ -std=c++14 -Wall -Wextra -Iinclude src/*.cpp main.cpp -o main.exe
```

### Run
```bash
# Terminal Application
.\main.exe

# Or Automated Demo
.\run_demo.bat

# Web Dashboard (open in any browser)
start web\index.html
```

# Linux / macOS
./main.exe
```

---

## 🖥️ Interactive System Menu

```text
============================= MAIN MENU =============================
  --- [Setup & Records] ---
  1.  Register a Hiker
  2.  Add a Trail Connection (Graph Edge)
  3.  View Full System Status (Hikers, Units, Network Map)

  --- [Navigation & Tracking] ---
  4.  Find Shortest Route (Dijkstra Algorithm)
  5.  Manual Hiker Location Update (Direct Check-in)
  6.  Plan & Start Hiker Trek (Route Assignment)
  7.  Simulate Auto Location Tracking (Live Checkpoint Crossings)
  8.  Ping Next Checkpoint Crossing (Single Sensor Event)
  9.  View Hiker Checkpoint Crossing Log & Travel History

  --- [Safety, Hazards & SOS] ---
  10. SOS - Dispatch Nearest Rescue Team
  11. Recall a Rescue Team to Base
  12. Trail Closure & Hazard Condition Management
  13. Broadcast Emergency Safety Alert to All Hikers

  --- [Persistence & Web Export] ---
  14. Save System State to Disk
  15. Load System State from Disk
  16. Export State to JSON (For Website Dashboard)
  17. Exit System
=====================================================================
```

---

## 🗺️ Demo Trail Network

Loaded automatically at startup (Dehradun–Mussoorie foothills):

```text
Old Rajpur ──3.2── Shikhar Falls
Old Rajpur ──6.5── Jharipani ──5.5── Mussoorie Library Chowk
Old Rajpur ──6.0── Robber's Cave ──5.5── Kimadi Village
Kimadi Village ──6.2── Hathipaon
Mussoorie Library Chowk ──4.0── Hathipaon
Hathipaon ──2.5── George Everest Peak ──4.2── Benog Tibba
Hathipaon ──8.0── Bhadraj Temple

Nag Tibba Base  (intentionally isolated - demonstrates the "no route" case)
```

---

## ✅ Validation & Test Cases

The system rejects invalid input rather than corrupting the graph:

| Test | Input | Expected result |
|------|-------|-----------------|
| Shortest path | `Old Rajpur` → `Benog Tibba` | 22.70 km, 6 checkpoints |
| Unreachable node | `Old Rajpur` → `Nag Tibba Base` | "No trail connects…" |
| Nearest-team dispatch | SOS for `H-101` | Selects `RT-QRT-02` (2.50 km) |
| Duplicate trail | `Old Rajpur` → `Shikhar Falls` again | Rejected, graph unchanged |
| Self-loop | `Hathipaon` → `Hathipaon` | Rejected |
| Blank checkpoint name | *(empty)* → *(empty)* | Rejected |
| Duplicate hiker ID | Register `H-101` twice | Rejected |
| Unknown hiker | SOS for `H-999` | "No hiker found" |
| Stream closed (EOF) | Piped input ends early | Clean shutdown, no infinite loop |

---

## 👤 TEAM MEMBERS
* **Vidarbh Dabral** — ML2/67 — Semester 03
* **Shorya Negi** — DS1/41 — Roll No. 221106 — Semester 03
* **Arnav Negi** — G/16 — Semester 03
* **Rajat Pant** — ML5/45 — Semester 03
