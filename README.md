# Trail Safety and Rescue System

A C++ application for offroad trails and mountain rescue operations. It models a trail network as a weighted graph (adjacency list) and uses Dijkstra's algorithm to find shortest routes and dispatch the nearest available rescue team to a hiker in distress.

Developed as a 3rd Semester B.Tech CSE Project-Based Learning (PBL) assignment.

---

## Project Modules and Ownership

main.cpp and the codebase are divided into four core sections across team members:

| Section | Contents | Owner |
|---|---|---|
| Section A | Entity layer: User, Hiker, RescueTeam, UserLinkedList, RescueTeamLinkedList | Arnav Negi |
| Section B | Graph and Dijkstra: TrailEdge, PathResult, TrailGraph, dynamic hazard routing | Vidarbh Dabral |
| Section C | Facade system: TrailSafetySystem, auto-tracking telemetry, SOS dispatch | Shorya Negi |
| Section D | Menu UI, animated progress bar, input validation, web dashboard, and main driver | Rajat Pant |

---

## Architecture and Data Structures

### 1. Object-Oriented Design (Owner: Arnav Negi)
* Abstraction: `User` is an Abstract Base Class with pure virtual methods (`displayDetails()`, `getRole()`).
* Encapsulation: Member variables are private or protected, accessed through const getters and validating setters.
* Inheritance: `Hiker` publicly inherits `User` and adds checkpoint telemetry, trek state, and travel history.
* Polymorphism: Runtime dispatch through base class pointers.
* RAII: Modern smart pointers (`std::unique_ptr`) guarantee memory safety and clean destruction.

### 2. Custom Singly Linked Lists (Owner: Arnav Negi)
* `UserLinkedList`: Stores registered hikers using dynamic heap-allocated `UserNode` pointers.
* `RescueTeamLinkedList`: Stores rescue response units using dynamic heap-allocated `RescueTeamNode` pointers.
* `CrossingLinkedList`: Stores hiker checkpoint crossing audit telemetry logs as linked nodes.
* Provides O(1) appending at tail and clean pointer-based traversal.

### 3. Graph as Adjacency List (Owner: Vidarbh Dabral)
```cpp
std::unordered_map<std::string, std::vector<TrailEdge>>
```
* Mountain trail networks are sparse (|E| << |V|^2). An adjacency list uses O(V + E) space compared to O(V^2) for an adjacency matrix.
* Hash indexing gives O(1) average checkpoint lookup using readable checkpoint names.

### 4. Dijkstra's Shortest Path Algorithm (Owner: Vidarbh Dabral)
* Implemented in `TrailGraph::findShortestPath()`.
* Uses a min-heap priority queue (`std::priority_queue<DistNode, vector<DistNode>, greater<DistNode>>`) to process the closest unsettled node first.
* Time complexity: O((V + E) log V).
* Dynamic hazard bypass: Automatically detects and avoids `CLOSED` trail edges while flagging `CAUTION` segments.

### 5. Facade System, Auto-Tracking and SOS (Owner: Shorya Negi)
* `TrailSafetySystem` coordinates between the graph, linked lists of hikers, and rescue stations.
* Automatic Location Tracking: Updates the hiker's checkpoint as they cross sensors along a planned trek.
* SOS Emergency Dispatch: Dispatches the closest available team to the hiker's latest verified location.

### 6. Interactive Terminal UI and Web Dashboard (Owner: Rajat Pant)
* Terminal UI with numeric stream cleansing to prevent infinite loops on invalid input.
* Animated ASCII progress bar for live trail traversal simulation.
* Interactive web dashboard in `web/index.html` displaying network visualization on an HTML5 canvas.

---

## Project Structure

```text
.
|-- include/
|   |-- Common.hpp               # Time, sleep and infinity definitions
|   |-- User.hpp                 # Abstract base class
|   |-- Hiker.hpp                # Hiker entity and CrossingLinkedList history
|   |-- RescueTeam.hpp           # Rescue unit entity and dispatch status
|   |-- UserLinkedList.hpp       # Custom Singly Linked List for Hikers
|   |-- RescueTeamLinkedList.hpp # Custom Singly Linked List for Rescue Teams
|   |-- TrailGraph.hpp           # Adjacency list and Dijkstra header
|   |-- TrailSafetySystem.hpp    # Facade system and auto-tracker
|   `-- UI.hpp                   # Progress bar and terminal menu declarations
|-- src/
|   |-- TrailGraph.cpp           # Graph algorithms and hazard bypass
|   |-- TrailSafetySystem.cpp    # System implementation and SOS dispatch
|   `-- UI.cpp                   # UI functions and validation
|-- web/
|   `-- index.html               # Interactive visual web dashboard (Canvas + JS)
|-- main.cpp                     # Application entry point
|-- build.bat                    # Windows build script
|-- Makefile                     # GCC build script
|-- trail_network.json           # Web dashboard network data
|-- trail_safety_data.txt        # System state persistence file
|-- .gitignore                   # Build artifact exclusions
`-- README.md                    # Project documentation
```

---

## How to Build and Run

### Prerequisites
A C++ compiler supporting C++14 or newer (MinGW g++, GCC, or Clang).

### Compile
On Windows:
```bash
.\build.bat
```

Or using g++ directly:
```bash
g++ -std=c++14 -Wall -Wextra -Iinclude src/*.cpp main.cpp -o main.exe
```

Using make:
```bash
make
```

### Run
Terminal application:
```bash
# Windows
.\main.exe

# Linux / macOS
./main.exe
```

Web Dashboard:
Open `web/index.html` in any web browser.

---

## Interactive Menu

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

  14. Exit System
=====================================================================
```

---

## Demo Trail Network

Pre-loaded network (Dehradun - Mussoorie foothills):

```text
Old Rajpur -- 3.2 km -- Shikhar Falls
Old Rajpur -- 6.5 km -- Jharipani -- 5.5 km -- Mussoorie Library Chowk
Old Rajpur -- 6.0 km -- Robber's Cave -- 5.5 km -- Kimadi Village
Kimadi Village -- 6.2 km -- Hathipaon
Mussoorie Library Chowk -- 4.0 km -- Hathipaon
Hathipaon -- 2.5 km -- George Everest Peak -- 4.2 km -- Benog Tibba
Hathipaon -- 8.0 km -- Bhadraj Temple

Nag Tibba Base (isolated node - demonstrates unreachable destination)
```

---

## Validation and Edge Cases Handled

* Unreachable node: Handled cleanly when no path connects start and destination.
* Duplicate trail / self-loop: Rejected without modifying the graph.
* Blank input: Checkpoint names and hiker details cannot be blank.
* Duplicate hiker ID: Rejected before insertion into the linked list.
* Stream closed / EOF: Handled cleanly without infinite loops.

---

## Team Members

* Vidarbh Dabral - ML2/67 - Semester 03
* Shorya Negi - DS1/41 - Roll No. 221106 - Semester 03
* Arnav Negi - G/16 - Semester 03
* Rajat Pant - ML5/45 - Semester 03
