#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <iomanip>
#include <limits>

using namespace std;

class User {
protected:
    string userId;
    string name;
    string contactNumber;

public:
    virtual ~User() = default;

    User(string id, string name, string contact)
        : userId(move(id)), name(move(name)), contactNumber(move(contact)) {}

    virtual void displayDetails() const = 0;
    virtual string getRole() const = 0;

    string getId() const { return userId; }
    string getName() const { return name; }
    string getContact() const { return contactNumber; }

    void setName(const string& newName) {
        if (!newName.empty()) {
            name = newName;
        }
    }

    void setContact(const string& newContact) {
        if (!newContact.empty()) {
            contactNumber = newContact;
        }
    }
};

class Hiker : public User {
private:
    string emergencyContact;
    string currentCheckpoint;

public:
    Hiker(string id, string name, string contact, 
          string emergencyContact, string initialCheckpoint = "Old Rajpur")
        : User(move(id), move(name), move(contact)),
          emergencyContact(move(emergencyContact)),
          currentCheckpoint(move(initialCheckpoint)) {}

    string getRole() const override {
        return "Hiker";
    }

    void displayDetails() const override {
        cout << "  [Hiker ID]         : " << userId << "\n"
             << "  Name               : " << name << "\n"
             << "  Contact            : " << contactNumber << "\n"
             << "  Emergency Contact  : " << emergencyContact << "\n"
             << "  Current Location   : " << currentCheckpoint << "\n";
    }

    string getEmergencyContact() const { return emergencyContact; }
    string getCurrentCheckpoint() const { return currentCheckpoint; }

    void setEmergencyContact(const string& contact) { emergencyContact = contact; }
    void setCurrentCheckpoint(const string& checkpoint) { currentCheckpoint = checkpoint; }
};

class RescueTeam {
private:
    string teamId;
    string teamLeader;
    string baseLocation;
    bool isAvailable;

public:
    RescueTeam(string id, string leader, string base)
        : teamId(move(id)), teamLeader(move(leader)), 
          baseLocation(move(base)), isAvailable(true) {}

    void displayTeam() const {
        cout << "  [Team ID]    : " << teamId << "\n"
             << "  Team Leader  : " << teamLeader << "\n"
             << "  Base Camp    : " << baseLocation << "\n"
             << "  Status       : " << (isAvailable ? "Ready / On Patrol" : "Dispatched to Emergency") << "\n";
    }

    string getTeamId() const { return teamId; }
    string getLeader() const { return teamLeader; }
    string getBaseLocation() const { return baseLocation; }
    bool getAvailability() const { return isAvailable; }

    void setAvailability(bool status) { isAvailable = status; }
    void setBaseLocation(const string& newBase) { baseLocation = newBase; }
};

struct TrailEdge {
    string destination;
    double distanceKm;

    TrailEdge(string dest, double dist)
        : destination(move(dest)), distanceKm(dist) {}
};

class TrailGraph {
private:
    unordered_map<string, vector<TrailEdge>> adjList;

public:
    TrailGraph() = default;

    void addCheckpoint(const string& checkpointName) {
        if (checkpointName.empty()) return;
        if (adjList.find(checkpointName) == adjList.end()) {
            adjList[checkpointName] = vector<TrailEdge>();
        }
    }

    void addTrail(const string& from, const string& to, double distanceKm, bool bidirectional = true) {
        if (from.empty() || to.empty() || distanceKm <= 0.0) {
            cout << "  [!] Error: Invalid trail specifications. Distance must be positive.\n";
            return;
        }

        addCheckpoint(from);
        addCheckpoint(to);

        adjList[from].emplace_back(to, distanceKm);
        if (bidirectional) {
            adjList[to].emplace_back(from, distanceKm);
        }
    }

    void displayMap() const {
        if (adjList.empty()) {
            cout << "  (Trail network is empty. Please add checkpoints and trails.)\n";
            return;
        }

        cout << "-------------------------------------------------------------\n";
        cout << left << setw(22) << "Checkpoint (Node)" << "Connected Trails (Neighbor [Distance])\n";
        cout << "-------------------------------------------------------------\n";

        for (const auto& pair : adjList) {
            const string& checkpoint = pair.first;
            const vector<TrailEdge>& edges = pair.second;

            cout << "  * " << left << setw(20) << checkpoint << " -> ";
            if (edges.empty()) {
                cout << "[Isolated Checkpoint - No active trails]";
            } else {
                for (size_t i = 0; i < edges.size(); ++i) {
                    cout << edges[i].destination << " [" << edges[i].distanceKm << " km]";
                    if (i + 1 < edges.size()) {
                        cout << " , ";
                    }
                }
            }
            cout << "\n";
        }
        cout << "-------------------------------------------------------------\n";
    }

    bool hasCheckpoint(const string& name) const {
        return adjList.find(name) != adjList.end();
    }

    size_t getCheckpointCount() const {
        return adjList.size();
    }
};

class TrailSafetySystem {
private:
    vector<unique_ptr<User>> registeredUsers;
    vector<RescueTeam> rescueTeams;
    TrailGraph trailMap;

public:
    TrailSafetySystem() = default;

    TrailSafetySystem(const TrailSafetySystem&) = delete;
    TrailSafetySystem& operator=(const TrailSafetySystem&) = delete;

    void registerHiker(const string& id, const string& name, 
                       const string& contact, const string& emergencyContact,
                       const string& startingCheckpoint = "Old Rajpur") {
        trailMap.addCheckpoint(startingCheckpoint);
        registeredUsers.push_back(
            make_unique<Hiker>(id, name, contact, emergencyContact, startingCheckpoint)
        );
        cout << "\n[OK] Successfully registered Hiker: \"" << name << "\" (ID: " << id << ")\n";
    }

    void registerRescueTeam(const RescueTeam& team) {
        trailMap.addCheckpoint(team.getBaseLocation());
        rescueTeams.push_back(team);
    }

    void addTrailConnection(const string& from, const string& to, 
                            double distanceKm, bool bidirectional = true) {
        trailMap.addTrail(from, to, distanceKm, bidirectional);
        cout << "\n[OK] Trail successfully established: " 
             << from << " <---> " << to << " (" << distanceKm << " km)\n";
    }

    void displaySystemStatus() const {
        cout << "\n=============================================================\n";
        cout << "          TRAIL SAFETY & RESCUE SYSTEM STATUS REPORT         \n";
        cout << "=============================================================\n";

        cout << "\n[SECTION 1: REGISTERED HIKERS & USERS] (Total: " << registeredUsers.size() << ")\n";
        if (registeredUsers.empty()) {
            cout << "  No users registered yet.\n";
        } else {
            for (size_t i = 0; i < registeredUsers.size(); ++i) {
                cout << "\n-- Record #" << (i + 1) << " [" << registeredUsers[i]->getRole() << "] --\n";
                registeredUsers[i]->displayDetails();
            }
        }

        cout << "\n[SECTION 2: RESCUE & RANGER STATIONS] (Total: " << rescueTeams.size() << ")\n";
        if (rescueTeams.empty()) {
            cout << "  No rescue teams currently registered.\n";
        } else {
            for (size_t i = 0; i < rescueTeams.size(); ++i) {
                cout << "\n-- Response Unit #" << (i + 1) << " --\n";
                rescueTeams[i].displayTeam();
            }
        }

        cout << "\n[SECTION 3: TRAIL NETWORK MAP (GRAPH ADJACENCY LIST)]\n";
        trailMap.displayMap();
    }

    void seedInitialDemoData() {
        // Dehradun & Mussoorie Foothill Trail Network
        trailMap.addTrail("Old Rajpur", "Shikhar Falls", 3.2);
        trailMap.addTrail("Old Rajpur", "Jharipani", 6.5);
        trailMap.addTrail("Jharipani", "Mussoorie Library Chowk", 5.5);
        trailMap.addTrail("Old Rajpur", "Robber's Cave (Guchhupani)", 6.0);
        trailMap.addTrail("Robber's Cave (Guchhupani)", "Kimadi Village", 5.5);
        trailMap.addTrail("Kimadi Village", "Hathipaon", 6.2);
        trailMap.addTrail("Mussoorie Library Chowk", "Hathipaon", 4.0);
        trailMap.addTrail("Hathipaon", "George Everest Peak", 2.5);
        trailMap.addTrail("Hathipaon", "Bhadraj Temple", 8.0);
        trailMap.addTrail("George Everest Peak", "Benog Tibba", 4.2);

        registerRescueTeam(RescueTeam("RT-SDRF-01", "Inspector Rajesh Rawat", "Old Rajpur"));
        registerRescueTeam(RescueTeam("RT-QRT-02", "Officer Sunita Thapa", "Hathipaon"));

        registeredUsers.push_back(
            make_unique<Hiker>("H-101", "Aarav Sharma", "+91-98765-43210", "+91-98765-99887", "George Everest Peak")
        );
    }
};

void clearCin() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main() {
    TrailSafetySystem safetySystem;
    safetySystem.seedInitialDemoData();

    int choice = 0;

    cout << "=============================================================\n";
    cout << "    TRAIL SAFETY & RESCUE MANAGEMENT SYSTEM (PHASE 1)        \n";
    cout << "   B.Tech 3rd Semester PBL - Core Data Structures & OOP      \n";
    cout << "=============================================================\n";
    cout << "[System]: Demo network loaded. Ready for evaluation.\n";

    while (true) {
        cout << "\n======================== MAIN MENU ========================\n";
        cout << "  1. Register a Hiker\n";
        cout << "  2. Add a Trail to the Map (Graph Edge)\n";
        cout << "  3. View Current System Status (Users + Graph)\n";
        cout << "  4. Exit\n";
        cout << "===========================================================\n";
        cout << "Enter selection (1-4): ";

        if (!(cin >> choice)) {
            cout << "\n[!] Invalid input. Please enter a valid integer (1-4).\n";
            clearCin();
            continue;
        }

        clearCin();

        switch (choice) {
            case 1: {
                string id, name, contact, emergencyContact, startCheckpoint;

                cout << "\n--- REGISTER NEW HIKER ---\n";
                cout << "Enter Hiker ID (e.g., H-102): ";
                getline(cin, id);

                cout << "Enter Full Name: ";
                getline(cin, name);

                cout << "Enter Contact Number: ";
                getline(cin, contact);

                cout << "Enter Emergency Contact Number: ";
                getline(cin, emergencyContact);

                cout << "Enter Starting Checkpoint (default: Old Rajpur): ";
                getline(cin, startCheckpoint);
                if (startCheckpoint.empty()) {
                    startCheckpoint = "Old Rajpur";
                }

                safetySystem.registerHiker(id, name, contact, emergencyContact, startCheckpoint);
                break;
            }

            case 2: {
                string from, to;
                double distance = 0.0;

                cout << "\n--- ADD TRAIL CONNECTION (GRAPH EDGE) ---\n";
                cout << "Enter Origin Checkpoint: ";
                getline(cin, from);

                cout << "Enter Destination Checkpoint: ";
                getline(cin, to);

                cout << "Enter Trail Distance (km): ";
                while (!(cin >> distance) || distance <= 0.0) {
                    cout << "[!] Distance must be a positive number. Try again: ";
                    clearCin();
                }
                clearCin();

                cout << "Is this trail bidirectional? (y/n, default: y): ";
                string biStr;
                getline(cin, biStr);
                bool bidirectional = true;
                if (!biStr.empty() && (biStr[0] == 'n' || biStr[0] == 'N')) {
                    bidirectional = false;
                }

                safetySystem.addTrailConnection(from, to, distance, bidirectional);
                break;
            }

            case 3: {
                safetySystem.displaySystemStatus();
                break;
            }

            case 4: {
                cout << "\nExiting Trail Safety and Rescue System. Have a safe journey!\n";
                return 0;
            }

            default:
                cout << "\n[!] Selection out of range. Please choose an option between 1 and 4.\n";
                break;
        }
    }

    return 0;
}
