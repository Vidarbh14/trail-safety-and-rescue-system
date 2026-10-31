// =============================================================================
//  SECTION D  -  UI IMPLEMENTATION & INPUT VALIDATION
//  Author: Rajat Pant (Roll No. ML5/45)
// =============================================================================

#include "UI.hpp"
#include <iostream>
#include <iomanip>
#include <limits>

using namespace std;

void renderProgressBar(int percentage, const string& actionLabel, int width) {
    if (percentage < 0) percentage = 0;
    if (percentage > 100) percentage = 100;

    cout << "  [" << actionLabel << "] [";
    int pos = width * percentage / 100;
    for (int i = 0; i < width; ++i) {
        if (i < pos) cout << "=";
        else if (i == pos) cout << ">";
        else cout << " ";
    }
    cout << "] " << setw(3) << percentage << "%\r";
    cout.flush();
}

void clearCin() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

bool readPositiveDouble(double& value) {
    while (true) {
        if (cin >> value) {
            clearCin();
            if (value > 0.0) return true;
            cout << "[!] Distance must be greater than 0. Try again: ";
        } else {
            if (cin.eof()) return false;
            cout << "[!] Please enter a valid numerical value: ";
            clearCin();
        }
    }
}

void printMenu() {
    cout << "\n============================= MAIN MENU =============================\n";
    cout << "  --- [Setup & Records] ---\n";
    cout << "  1.  Register a Hiker\n";
    cout << "  2.  Add a Trail Connection (Graph Edge)\n";
    cout << "  3.  View Full System Status (Hikers, Units, Network Map)\n";
    cout << "\n  --- [Navigation & Tracking] ---\n";
    cout << "  4.  Find Shortest Route (Dijkstra Algorithm)\n";
    cout << "  5.  Manual Hiker Location Update (Direct Check-in)\n";
    cout << "  6.  Plan & Start Hiker Trek (Route Assignment)\n";
    cout << "  7.  Simulate Auto Location Tracking (Live Animated Progress)\n";
    cout << "  8.  Ping Next Checkpoint Crossing (Single Sensor Event)\n";
    cout << "  9.  View Hiker Checkpoint Crossing Log & Travel History\n";
    cout << "\n  --- [Safety, Hazards & SOS] ---\n";
    cout << "  10. SOS - Dispatch Nearest Rescue Team\n";
    cout << "  11. Recall a Rescue Team to Base\n";
    cout << "  12. Trail Closure & Hazard Condition Management\n";
    cout << "  13. Broadcast Emergency Safety Alert to All Hikers\n";
    cout << "\n  --- [Persistence & Web Export] ---\n";
    cout << "  14. Save System State to Disk\n";
    cout << "  15. Load System State from Disk\n";
    cout << "  16. Export State to JSON (For Website Dashboard)\n";
    cout << "  17. Exit System\n";
    cout << "=====================================================================\n";
    cout << "Enter selection (1-17): ";
}
