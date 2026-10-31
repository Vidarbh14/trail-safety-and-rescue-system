// =============================================================================
//  AUTOMATED TEST SUITE & VERIFICATION HARNESS
//  B.Tech 3rd Semester PBL Project : Mountain Trail Safety & Rescue System
//
//  Build & Run:
//    g++ -std=c++14 -Wall -Wextra -Iinclude src/TrailGraph.cpp src/TrailSafetySystem.cpp src/UI.cpp tests/test_suite.cpp -o test_suite.exe
//    .\test_suite.exe
// =============================================================================

#include "TrailSafetySystem.hpp"
#include "UserLinkedList.hpp"
#include <iostream>
#include <cassert>
#include <cmath>

using namespace std;

void testEntityAndLinkedList() {
    cout << "[TEST 1] Custom UserLinkedList & OOP Hierarchy... ";
    UserLinkedList list;
    assert(list.empty() == true);
    assert(list.size() == 0);

    list.append(make_unique<Hiker>("H-TEST1", "Alice Walker", "+91-11111-22222", "+91-33333-44444", "Old Rajpur", "Asthma Inhaler"));
    list.append(make_unique<Hiker>("H-TEST2", "Bob Trekk", "+91-55555-66666", "+91-77777-88888", "Jharipani"));

    assert(list.size() == 2);
    assert(list.empty() == false);

    Hiker* h1 = list.findHikerById("H-TEST1");
    assert(h1 != nullptr);
    assert(h1->getName() == "Alice Walker");
    assert(h1->getRole() == "Hiker");
    assert(h1->getCurrentCheckpoint() == "Old Rajpur");

    Hiker* h2 = list.findHikerById("H-TEST2");
    assert(h2 != nullptr);
    assert(h2->getName() == "Bob Trekk");

    Hiker* notFound = list.findHikerById("NON_EXISTENT");
    assert(notFound == nullptr);

    list.clear();
    assert(list.empty() == true);
    assert(list.size() == 0);
    assert(list.getHead() == nullptr);

    cout << "PASSED" << endl;
}

void testGraphAndDijkstra() {
    cout << "[TEST 2] Graph & Dijkstra Shortest Path... ";
    TrailGraph graph;
    graph.addTrail("A", "B", 5.0);
    graph.addTrail("B", "C", 4.0);
    graph.addTrail("A", "C", 12.0); // Longer direct path
    graph.addCheckpoint("Isolated");

    PathResult res = graph.findShortestPath("A", "C", true);
    assert(res.found == true);
    assert(fabs(res.totalDistance - 9.0) < 0.001); // Path A -> B -> C = 9.0 km
    assert(res.path.size() == 3);
    assert(res.path[0] == "A" && res.path[1] == "B" && res.path[2] == "C");

    PathResult unreachable = graph.findShortestPath("A", "Isolated", true);
    assert(unreachable.found == false);
    cout << "PASSED" << endl;
}

void testHazardAvoidance() {
    cout << "[TEST 3] Dynamic Hazard Avoidance... ";
    TrailGraph graph;
    graph.addTrail("Node1", "Node2", 3.0);
    graph.addTrail("Node2", "Node3", 3.0);
    graph.addTrail("Node1", "Node3", 10.0);

    // Close direct edge between Node1 and Node2
    graph.updateTrailStatus("Node1", "Node2", TrailStatus::CLOSED, "Landslide", 999.0);

    PathResult res = graph.findShortestPath("Node1", "Node3", true);
    assert(res.found == true);
    // Should detour around closed edge and take direct 10.0 km route
    assert(fabs(res.totalDistance - 10.0) < 0.001);
    cout << "PASSED" << endl;
}

void testAutomaticTracking() {
    cout << "[TEST 4] Automatic Checkpoint Tracking Telemetry... ";
    TrailSafetySystem system;
    system.registerHiker("H-TRACK", "Track Runner", "123", "456", "Old Rajpur");
    system.addTrailConnection("Old Rajpur", "Jharipani", 6.5);
    system.addTrailConnection("Jharipani", "Mussoorie Library Chowk", 5.5);

    bool trekStarted = system.startHikerTrek("H-TRACK", "Mussoorie Library Chowk");
    assert(trekStarted == true);

    Hiker* h = system.findHikerById("H-TRACK");
    assert(h != nullptr);
    assert(h->isTrekActive() == true);

    // Cross first checkpoint
    bool ping1 = system.advanceHikerSingleCheckpoint("H-TRACK");
    assert(ping1 == true);
    assert(h->getCurrentCheckpoint() == "Jharipani");
    assert(fabs(h->getTotalDistanceCovered() - 6.5) < 0.001);

    // Cross final checkpoint
    bool ping2 = system.advanceHikerSingleCheckpoint("H-TRACK");
    assert(ping2 == true);
    assert(h->getCurrentCheckpoint() == "Mussoorie Library Chowk");
    assert(fabs(h->getTotalDistanceCovered() - 12.0) < 0.001);
    assert(h->isTrekActive() == false); // Completed trek
    cout << "PASSED" << endl;
}

void testPersistence() {
    cout << "[TEST 5] File I/O Persistence & JSON Export... ";
    TrailSafetySystem system1;
    system1.seedInitialDemoData();

    bool saved = system1.saveSystemState("test_data.txt");
    assert(saved == true);

    bool jsonExported = system1.exportStateToJson("test_network.json");
    assert(jsonExported == true);

    TrailSafetySystem system2;
    bool loaded = system2.loadSystemState("test_data.txt");
    assert(loaded == true);
    assert(system2.findHikerById("H-101") != nullptr);
    assert(system2.findHikerById("H-102") != nullptr);
    cout << "PASSED" << endl;
}

int main() {
    cout << "=============================================================\n";
    cout << "       RUNNING AUTOMATED TEST SUITE FOR TRAIL SYSTEM         \n";
    cout << "=============================================================\n";

    testEntityAndLinkedList();
    testGraphAndDijkstra();
    testHazardAvoidance();
    testAutomaticTracking();
    testPersistence();

    cout << "=============================================================\n";
    cout << "  ALL 5 VERIFICATION SUITES COMPLETED WITH 100% SUCCESS!     \n";
    cout << "=============================================================\n";
    return 0;
}
