# 🏔️ Trail Safety and Rescue System

A C++ terminal application for offroad trails and mountain rescue operations. It models a trail network as a **weighted graph (adjacency list)** and uses **Dijkstra's algorithm** to find shortest routes and dispatch the nearest available rescue team to a hiker in distress.

Developed as a **3rd Semester B.Tech CSE Project-Based Learning (PBL)** assignment.

---

## Project Status

| Phase | Target | Status |
|-------|--------|--------|
| **Phase 1** | Graph storage, OOP hierarchy, facade | ✅ Complete |
| **Phase 2** | Dijkstra shortest path + SOS rescue dispatch | ✅ Complete |
| Phase 3 | File I/O persistence (CSV/JSON save & load) | ⬜ Planned |
| Phase 4 | Multi-hiker alerts, trail closures, risk scoring | ⬜ Planned |

---

## 🏛️ Architecture

### 1. Object-Oriented Design

* **Abstraction** — `User` is an Abstract Base Class with pure virtual methods (`displayDetails()`, `getRole()`), defining the contract for every participant.
* **Encapsulation** — Internal state is `private`/`protected` and reached only through const-correct getters and *validating* mutators (blank values are rejected, not stored).
* **Inheritance** — `Hiker` publicly inherits `User` (an *is-a* relationship) and adds emergency telemetry.
* **Polymorphism** — Runtime dispatch via virtual functions over `std::vector<std::unique_ptr<User>>`.
* **RAII** — `std::unique_ptr` guarantees leak-free ownership; a virtual destructor ensures derived objects tear down correctly through base pointers.
* **Facade** — `TrailSafetySystem` is the single entry point coordinating entities and the graph.

### 2. Data Structure — Graph as Adjacency List

```
std::unordered_map<std::string, std::vector<TrailEdge>>
```

* **Why an adjacency list?** Trail networks are **sparse** (|E| ≪ |V|²). An adjacency list costs **O(V + E)** memory versus **O(V²)** for a matrix.
* **Why `unordered_map`?** Hash indexing gives **amortised O(1)** checkpoint lookup using readable string names rather than integer indices.

### 3. Algorithm — Dijkstra's Shortest Path

Implemented in `TrailGraph::findShortestPath()`.

* **Min-heap:** `std::priority_queue<DistNode, vector<DistNode>, greater<DistNode>>` always yields the nearest unsettled checkpoint.
* **Complexity:** **O((V + E) log V)**.
* **Why Dijkstra and not BFS?** BFS minimises the *number of checkpoints*, not distance. The shortest *hop count* is often the longer *walk* — on this map, the 3-hop route to Benog Tibba is 24.4 km while the 5-hop route is 22.7 km.
* **Why not Bellman–Ford?** All trail distances are positive, so negative-weight handling is unnecessary and would cost O(V·E).
* **Lazy deletion:** A checkpoint may enter the heap several times; a `settled` map ensures only the first (smallest) pop is processed.
* **Path reconstruction:** A `previous` map records how each checkpoint was reached; the chain is walked backwards from the destination and reversed.

---

## 🚑 Phase 2 Feature — SOS Dispatch

`dispatchNearestTeam(hikerId)` runs Dijkstra **once per available rescue team**, from each team's base camp to the hiker's current checkpoint, then commits whichever team is closest by actual trail distance and marks it unavailable.

Live example from the demo data — hiker `H-101` at *George Everest Peak*:

```
    - RT-SDRF-01 : 18.50 km away
    - RT-QRT-02 : 2.50 km away
-------------------------------------------------------------
  >>> DISPATCHING RT-QRT-02 <<<
  Led by : Officer Sunita Thapa
  From   : Hathipaon
  Route   : Hathipaon -> George Everest Peak
  Distance: 2.50 km
```

---

## 📂 Project Structure

```text
.
├── .gitignore    # Ignores build artifacts (*.exe, *.o)
├── README.md     # This file
└── main.cpp      # Complete implementation, divided into owned sections A–D
```

`main.cpp` is split into four clearly banded sections so each team member edits a separate region of the file:

| Section | Contents | Owner |
|---------|----------|-------|
| **A** | Entity layer — `User`, `Hiker`, `RescueTeam` | Arnav Negi |
| **B** | Graph + Dijkstra — `TrailEdge`, `PathResult`, `TrailGraph` | Vidarbh Dabral |
| **C** | Facade — `TrailSafetySystem` (dispatch + hiker management) | Shorya Negi / Arnav Negi |
| **D** | Menu, input validation, `main()` | Rajat Pant |

---

## 🚀 Getting Started

### Prerequisites
A C++ compiler supporting C++14 or newer (GCC/MinGW `g++`, Clang, or MSVC).

### Compile
```bash
g++ -std=c++14 -Wall -Wextra main.cpp -o main.exe
```
The project compiles with **zero warnings** under `-Wall -Wextra -Wshadow -pedantic`.

### Run
```bash
# Windows
.\main.exe

# Linux / macOS
./main.exe
```

---

## 🖥️ Menu

```text
======================== MAIN MENU ========================
  --- Phase 1 : Setup ---
  1. Register a Hiker
  2. Add a Trail to the Map (Graph Edge)
  3. View Full System Status
  --- Phase 2 : Routing & Rescue ---
  4. Find Shortest Route Between Checkpoints (Dijkstra)
  5. Update a Hiker's Current Location
  6. Update a Hiker's Contact Details
  7. SOS - Dispatch Nearest Rescue Team
  8. Recall a Rescue Team to Base
  9. Exit
===========================================================
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
* **Shorya Negi** — DS1/41 — Roll No. 21106 — Semester 03
* **Arnav Negi** — G/16 — Semester 03
* **Rajat Pant** — ML5/45 — Semester 03
