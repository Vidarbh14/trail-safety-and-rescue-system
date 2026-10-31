// =============================================================================
//  MOUNTAIN TRAIL SAFETY AND RESCUE MANAGEMENT SYSTEM
//  B.Tech 3rd Semester PBL Project : Data Structures (Graph/Dijkstra) + OOP (C++)
//
//  TEAM MEMBERS & ARCHITECTURAL OWNERSHIP:
//    - Arnav Negi    (Roll No. G/16)   : Section A - OOP Hierarchy & Hiker Telemetry
//    - Vidarbh Dabral(Roll No. ML2/67) : Section B - Graph Data Structure & Dijkstra
//    - Shorya Negi   (Roll No. 221106) : Section C - Facade System, Tracker & File I/O
//    - Rajat Pant    (Roll No. ML5/45) : Section D - Terminal UI, Progress Bar & Driver
//
//  BUILD COMMAND:
//    g++ -std=c++14 -Wall -Wextra -Iinclude src/*.cpp main.cpp -o main.exe
// =============================================================================

#include "TrailSafetySystem.hpp"
#include "UI.hpp"
#include <iostream>
#include <string>

using namespace std;

int main() {
    TrailSafetySystem safetySystem;
    safetySystem.seedInitialDemoData();

    cout << "=====================================================================\n";
    cout << "      MOUNTAIN TRAIL SAFETY & RESCUE MANAGEMENT SYSTEM               \n";
    cout << "     B.Tech 3rd Semester PBL - Data Structures & OOP Project         \n";
    cout << "=====================================================================\n";
    cout << "[System]: Foothill trail network, telemetry sensors, and units loaded.\n";

    int choice = 0;

    while (true) {
        printMenu();

        if (!(cin >> choice)) {
            if (cin.eof()) {
                cout << "\n[System]: Stream closed. Exiting safely.\n";
                return 0;
            }
            cout << "\n[!] Invalid input. Please enter a number between 1 and 17.\n";
            clearCin();
            continue;
        }
        clearCin();

        switch (choice) {
            case 1: { // Register Hiker
                string id, name, contact, emergency, start, medical;
                cout << "\n--- REGISTER NEW HIKER ---\n";
                cout << "Hiker ID (e.g. H-103)      : "; getline(cin, id);
                cout << "Full Name                  : "; getline(cin, name);
                cout << "Contact Number             : "; getline(cin, contact);
                cout << "Emergency Contact          : "; getline(cin, emergency);
                cout << "Starting Checkpoint        : "; getline(cin, start);
                cout << "Medical Notes / Gear Info  : "; getline(cin, medical);

                if (start.empty()) start = "Old Rajpur";
                if (medical.empty()) medical = "None";

                safetySystem.registerHiker(id, name, contact, emergency, start, medical);
                break;
            }

            case 2: { // Add Trail Connection
                string from, to, biStr;
                double distance = 0.0;
                cout << "\n--- ADD TRAIL CONNECTION (GRAPH EDGE) ---\n";
                cout << "Origin Checkpoint          : "; getline(cin, from);
                cout << "Destination Checkpoint     : "; getline(cin, to);
                cout << "Trail Distance (km)        : ";

                if (!readPositiveDouble(distance)) return 0;

                cout << "Two-way trail? (y/n)       : "; getline(cin, biStr);
                bool bidirectional = !(!biStr.empty() && (biStr[0] == 'n' || biStr[0] == 'N'));

                safetySystem.addTrailConnection(from, to, distance, bidirectional);
                break;
            }

            case 3: { // Full System Status
                safetySystem.displaySystemStatus();
                break;
            }

            case 4: { // Dijkstra Route Finding
                string from, to;
                cout << "\n--- FIND SHORTEST ROUTE (DIJKSTRA) ---\n";
                cout << "Start Checkpoint           : "; getline(cin, from);
                cout << "Destination Checkpoint     : "; getline(cin, to);
                safetySystem.findRouteBetween(from, to);
                break;
            }

            case 5: { // Direct Manual Location Update
                string id, checkpoint;
                cout << "\n--- MANUAL LOCATION UPDATE ---\n";
                cout << "Hiker ID                   : "; getline(cin, id);
                cout << "New Checkpoint Name        : "; getline(cin, checkpoint);
                safetySystem.updateHikerLocation(id, checkpoint);
                break;
            }

            case 6: { // Plan & Start Hiker Trek
                string id, destination;
                cout << "\n--- PLAN & START HIKER TREK ---\n";
                cout << "Hiker ID                   : "; getline(cin, id);
                cout << "Target Destination         : "; getline(cin, destination);
                safetySystem.startHikerTrek(id, destination);
                break;
            }

            case 7: { // Simulate Automatic Location Tracking
                string id;
                cout << "\n--- AUTOMATIC LOCATION TRACKING SIMULATION ---\n";
                cout << "Hiker ID to Auto-Track     : "; getline(cin, id);
                safetySystem.runAutoTrackingSimulation(id);
                break;
            }

            case 8: { // Ping Single Checkpoint Sensor
                string id;
                cout << "\n--- PING NEXT CHECKPOINT (SINGLE SENSOR EVENT) ---\n";
                cout << "Hiker ID                   : "; getline(cin, id);
                safetySystem.advanceHikerSingleCheckpoint(id);
                break;
            }

            case 9: { // View Checkpoint Crossing Log
                string id;
                cout << "\n--- VIEW HIKER CHECKPOINT CROSSING LOG ---\n";
                cout << "Hiker ID                   : "; getline(cin, id);
                safetySystem.displayHikerCrossingLog(id);
                break;
            }

            case 10: { // SOS Dispatch
                string id;
                cout << "\n--- SOS EMERGENCY DISPATCH ---\n";
                cout << "Hiker ID in distress       : "; getline(cin, id);
                safetySystem.dispatchNearestTeam(id);
                break;
            }

            case 11: { // Recall Team
                string teamId;
                cout << "\n--- RECALL RESCUE TEAM ---\n";
                cout << "Team ID (e.g. RT-QRT-02)   : "; getline(cin, teamId);
                safetySystem.recallTeam(teamId);
                break;
            }

            case 12: { // Trail Closure & Hazard Management
                string from, to, note;
                int statChoice = 1;
                cout << "\n--- TRAIL CLOSURE & HAZARD CONDITION ---\n";
                cout << "Checkpoint 1               : "; getline(cin, from);
                cout << "Checkpoint 2               : "; getline(cin, to);
                cout << "Select Status (1=OPEN, 2=CAUTION, 3=CLOSED): ";
                if (!(cin >> statChoice)) {
                    clearCin();
                    break;
                }
                clearCin();
                cout << "Hazard Description / Reason: "; getline(cin, note);

                TrailStatus status = TrailStatus::OPEN;
                double multiplier = 1.0;
                if (statChoice == 2) {
                    status = TrailStatus::CAUTION;
                    multiplier = 1.5;
                } else if (statChoice == 3) {
                    status = TrailStatus::CLOSED;
                    multiplier = INF / 2.0;
                }

                safetySystem.setTrailHazardCondition(from, to, status, note, multiplier);
                break;
            }

            case 13: { // Broadcast Emergency Alert
                string msg;
                cout << "\n--- BROADCAST SAFETY ALERT TO ALL HIKERS ---\n";
                cout << "Enter Announcement Message : "; getline(cin, msg);
                safetySystem.broadcastEmergencyAlert(msg);
                break;
            }

            case 14: { // Save to Disk
                string fname;
                cout << "\n--- SAVE SYSTEM STATE TO FILE ---\n";
                cout << "Enter filename (default: trail_safety_data.txt): "; getline(cin, fname);
                if (fname.empty()) fname = "trail_safety_data.txt";
                safetySystem.saveSystemState(fname);
                break;
            }

            case 15: { // Load from Disk
                string fname;
                cout << "\n--- LOAD SYSTEM STATE FROM FILE ---\n";
                cout << "Enter filename (default: trail_safety_data.txt): "; getline(cin, fname);
                if (fname.empty()) fname = "trail_safety_data.txt";
                safetySystem.loadSystemState(fname);
                break;
            }

            case 16: { // Export to JSON
                string fname;
                cout << "\n--- EXPORT SYSTEM FOR WEB DASHBOARD ---\n";
                cout << "Enter JSON filename (default: trail_network.json): "; getline(cin, fname);
                if (fname.empty()) fname = "trail_network.json";
                safetySystem.exportStateToJson(fname);
                break;
            }

            case 17: { // Exit
                cout << "\nExiting Mountain Trail Safety & Rescue System. Safe journey!\n";
                return 0;
            }

            default:
                cout << "\n[!] Selection out of range. Please choose between 1 and 17.\n";
                break;
        }
    }

    return 0;
}
