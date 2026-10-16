// =============================================================================
//  TRAIL SAFETY AND RESCUE MANAGEMENT SYSTEM  -  PHASE 2
//  B.Tech 3rd Semester PBL : Data Structures (Graph) + OOP
//
//  Phase 1 : Graph storage (adjacency list) + OOP entity hierarchy
//  Phase 2 : Dijkstra shortest path + nearest-rescue-team dispatch (SOS)
//
//  Build : g++ -std=c++14 -Wall -Wextra main.cpp -o main.exe
//  Run   : .\main.exe
// =============================================================================

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <iomanip>
#include <limits>
#include <queue>        // priority_queue -> used by Dijkstra
#include <algorithm>    // reverse        -> used to rebuild the route

using namespace std;

// "Infinity" marker. Any checkpoint we have not reached yet starts at this
// value, so the first real distance we find is always smaller and wins.
const double INF = numeric_limits<double>::max();


// =============================================================================
//  SECTION A  -  ENTITY LAYER (OOP)
//  Owner: ARNAV NEGI
// =============================================================================

// Abstract Base Class. Cannot be created directly - it only defines the
// contract that every person in the system must follow.
class User {
protected:
    string userId;
    string name;
    string contactNumber;

public:
    virtual ~User() = default;

    User(string id, string personName, string contact)
        : userId(move(id)), name(move(personName)), contactNumber(move(contact)) {}

    // Pure virtual = every derived class MUST provide its own version.
    virtual void displayDetails() const = 0;
    virtual string getRole() const = 0;

    string getId() const { return userId; }
    string getName() const { return name; }
    string getContact() const { return contactNumber; }

    void setName(const string& newName) {
        if (!newName.empty()) name = newName;
    }

    void setContact(const string& newContact) {
        if (!newContact.empty()) contactNumber = newContact;
    }
};


class Hiker : public User {
private:
    string emergencyContact;
    string currentCheckpoint;   // where this hiker is RIGHT NOW on the trail

public:
    Hiker(string id, string personName, string contact,
          string emergency, string startCheckpoint)
        : User(move(id), move(personName), move(contact)),
          emergencyContact(move(emergency)),
          currentCheckpoint(move(startCheckpoint)) {}

    string getRole() const override { return "Hiker"; }

    void displayDetails() const override {
        cout << "  [Hiker ID]         : " << userId << "\n"
             << "  Name               : " << name << "\n"
             << "  Contact            : " << contactNumber << "\n"
             << "  Emergency Contact  : " << emergencyContact << "\n"
             << "  Current Location   : " << currentCheckpoint << "\n";
    }

    string getEmergencyContact() const { return emergencyContact; }
    string getCurrentCheckpoint() const { return currentCheckpoint; }

    void setEmergencyContact(const string& contact) {
        if (!contact.empty()) emergencyContact = contact;
    }

    void setCurrentCheckpoint(const string& checkpoint) {
        if (!checkpoint.empty()) currentCheckpoint = checkpoint;
    }
};


class RescueTeam {
private:
    string teamId;
    string teamLeader;
    string baseLocation;   // the checkpoint this team is stationed at
    bool isAvailable;      // false once dispatched to an emergency

public:
    RescueTeam(string id, string leader, string base)
        : teamId(move(id)), teamLeader(move(leader)),
          baseLocation(move(base)), isAvailable(true) {}

    void displayTeam() const {
        cout << "  [Team ID]    : " << teamId << "\n"
             << "  Team Leader  : " << teamLeader << "\n"
             << "  Base Camp    : " << baseLocation << "\n"
             << "  Status       : "
             << (isAvailable ? "Ready / On Patrol" : "DISPATCHED to emergency") << "\n";
    }

    string getTeamId() const { return teamId; }
    string getLeader() const { return teamLeader; }
    string getBaseLocation() const { return baseLocation; }
    bool getAvailability() const { return isAvailable; }

    void setAvailability(bool status) { isAvailable = status; }
};


// =============================================================================
//  SECTION B  -  GRAPH + DIJKSTRA
//  Owner: VIDARBH DABRAL
// =============================================================================

// One trail leaving a checkpoint: where it goes, and how long it is.
// This is a single entry inside the adjacency list.
struct TrailEdge {
    string destination;
    double distanceKm;

    TrailEdge(string dest, double dist)
        : destination(move(dest)), distanceKm(dist) {}
};


// What Dijkstra hands back to the caller.
struct PathResult {
    bool found;              // false if the two checkpoints are not connected
    double totalDistance;    // total km along the shortest route
    vector<string> path;     // the route itself, in walking order

    PathResult() : found(false), totalDistance(0.0) {}
};


// A (distance, checkpoint) pair for the min-heap.
// pair compares on .first first, so the heap orders by distance automatically.
using DistNode = pair<double, string>;


class TrailGraph {
private:
    // THE core data structure.
    // key   = checkpoint name
    // value = list of trails leaving that checkpoint
    unordered_map<string, vector<TrailEdge>> adjList;

public:
    TrailGraph() = default;

    void addCheckpoint(const string& checkpointName) {
        if (checkpointName.empty()) return;
        if (adjList.find(checkpointName) == adjList.end()) {
            adjList[checkpointName] = vector<TrailEdge>();
        }
    }

    // Adds a trail edge between checkpoints. Returns true if added successfully.
    bool addTrail(const string& from, const string& to,
                  double distanceKm, bool bidirectional = true) {

        if (from.empty() || to.empty()) {
            cout << "  [!] Rejected: checkpoint names cannot be blank.\n";
            return false;
        }
        if (distanceKm <= 0.0) {
            cout << "  [!] Rejected: distance must be greater than 0 km.\n";
            return false;
        }
        if (from == to) {
            cout << "  [!] Rejected: a trail cannot start and end at the same checkpoint.\n";
            return false;
        }
        if (trailExists(from, to)) {
            cout << "  [!] Rejected: a trail between \"" << from
                 << "\" and \"" << to << "\" already exists.\n";
            return false;
        }

        addCheckpoint(from);
        addCheckpoint(to);

        adjList[from].push_back(TrailEdge(to, distanceKm));
        if (bidirectional) {
            adjList[to].push_back(TrailEdge(from, distanceKm));
        }
        return true;
    }

    bool trailExists(const string& from, const string& to) const {
        auto it = adjList.find(from);
        if (it == adjList.end()) return false;

        for (size_t i = 0; i < it->second.size(); ++i) {
            if (it->second[i].destination == to) return true;
        }
        return false;
    }

    bool hasCheckpoint(const string& name) const {
        return adjList.find(name) != adjList.end();
    }

    size_t getCheckpointCount() const { return adjList.size(); }

    // Returns outgoing trail edges for a given checkpoint
    const vector<TrailEdge>& getNeighbors(const string& checkpoint) const {
        static const vector<TrailEdge> noTrails;   // returned for unknown names
        auto it = adjList.find(checkpoint);
        if (it == adjList.end()) return noTrails;
        return it->second;
    }

    // -------------------------------------------------------------------
    //  DIJKSTRA'S SHORTEST PATH ALGORITHM
    //
    //  Idea: always expand the closest checkpoint we have not settled yet.
    //  Because every trail length is positive, the first time we settle a
    //  checkpoint we already know its true shortest distance.
    //
    //  Complexity: O((V + E) log V)  -- the log V comes from the min-heap.
    // -------------------------------------------------------------------
    PathResult findShortestPath(const string& source, const string& destination) const {
        PathResult result;

        if (!hasCheckpoint(source) || !hasCheckpoint(destination)) {
            return result;    // found stays false
        }

        // STEP 1 -- assume every checkpoint is unreachable, except the start.
        unordered_map<string, double> dist;
        unordered_map<string, string> previous;   // used to rebuild the route
        unordered_map<string, bool> settled;

        for (const auto& entry : adjList) {
            dist[entry.first] = INF;
        }
        dist[source] = 0.0;

        // STEP 2 -- min-heap. greater<> turns the default max-heap into a
        // min-heap, so pq.top() is always the nearest unsettled checkpoint.
        priority_queue<DistNode, vector<DistNode>, greater<DistNode>> pq;
        pq.push(make_pair(0.0, source));

        while (!pq.empty()) {
            string current = pq.top().second;
            pq.pop();

            // We may have pushed the same checkpoint more than once with
            // different distances. Only the first (smallest) pop counts.
            if (settled[current]) continue;
            settled[current] = true;

            if (current == destination) break;   // done, no need to go further

            // STEP 3 -- relaxation: can we reach a neighbour more cheaply
            // by going through 'current'?
            const vector<TrailEdge>& trails = getNeighbors(current);
            for (size_t i = 0; i < trails.size(); ++i) {
                const string& nextStop = trails[i].destination;
                double newDistance = dist[current] + trails[i].distanceKm;

                if (newDistance < dist[nextStop]) {
                    dist[nextStop] = newDistance;
                    previous[nextStop] = current;
                    pq.push(make_pair(newDistance, nextStop));
                }
            }
        }

        if (dist[destination] >= INF) {
            return result;    // never reached -> no route exists
        }

        // STEP 4 -- walk the 'previous' chain backwards from the destination,
        // then flip it so it reads in walking order.
        vector<string> route;
        string step = destination;
        route.push_back(step);
        while (step != source) {
            step = previous[step];
            route.push_back(step);
        }
        reverse(route.begin(), route.end());

        result.found = true;
        result.totalDistance = dist[destination];
        result.path = route;
        return result;
    }

    void displayMap() const {
        if (adjList.empty()) {
            cout << "  (Trail network is empty.)\n";
            return;
        }

        cout << "-------------------------------------------------------------\n";
        cout << left << setw(22) << "Checkpoint (Node)"
             << "Connected Trails (Neighbour [Distance])\n";
        cout << "-------------------------------------------------------------\n";

        for (const auto& entry : adjList) {
            cout << "  * " << left << setw(20) << entry.first << " -> ";

            if (entry.second.empty()) {
                cout << "[Isolated - no trails yet]";
            } else {
                for (size_t i = 0; i < entry.second.size(); ++i) {
                    cout << entry.second[i].destination
                         << " [" << entry.second[i].distanceKm << " km]";
                    if (i + 1 < entry.second.size()) cout << " , ";
                }
            }
            cout << "\n";
        }
        cout << "-------------------------------------------------------------\n";
    }
};


// =============================================================================
//  SECTION C  -  SYSTEM FACADE (ties entities + graph together)
//  Owner: SHORYA NEGI  (rescue dispatch)  +  ARNAV NEGI  (hiker management)
// =============================================================================

class TrailSafetySystem {
private:
    vector<unique_ptr<User>> registeredUsers;   // unique_ptr = automatic cleanup
    vector<RescueTeam> rescueTeams;
    TrailGraph trailMap;

    // Prints a route in a readable way. Used by both the route finder
    // and the SOS dispatcher.
    void displayRoute(const PathResult& route) const {
        cout << "  Route   : ";
        for (size_t i = 0; i < route.path.size(); ++i) {
            cout << route.path[i];
            if (i + 1 < route.path.size()) cout << " -> ";
        }
        cout << "\n";
        cout << "  Distance: " << fixed << setprecision(2)
             << route.totalDistance << " km\n";
        cout << "  Stops   : " << route.path.size() << " checkpoints\n";
        cout.unsetf(ios::fixed);
    }

public:
    TrailSafetySystem() = default;

    // Disable copying due to unique_ptr ownership of polymorphic users
    TrailSafetySystem(const TrailSafetySystem&) = delete;
    TrailSafetySystem& operator=(const TrailSafetySystem&) = delete;

    // ---------------- HIKER MANAGEMENT  (Arnav) ----------------

    // Linear search by ID; dynamic_cast safely converts the base pointer to Hiker*
    Hiker* findHikerById(const string& id) const {
        for (size_t i = 0; i < registeredUsers.size(); ++i) {
            if (registeredUsers[i]->getId() == id) {
                return dynamic_cast<Hiker*>(registeredUsers[i].get());
            }
        }
        return nullptr;
    }

    bool registerHiker(const string& id, const string& name,
                       const string& contact, const string& emergencyContact,
                       const string& startingCheckpoint) {

        if (id.empty() || name.empty()) {
            cout << "\n[FAILED] Hiker ID and Name are required.\n";
            return false;
        }
        if (findHikerById(id) != nullptr) {
            cout << "\n[FAILED] Hiker ID \"" << id << "\" is already registered.\n";
            return false;
        }

        trailMap.addCheckpoint(startingCheckpoint);
        registeredUsers.push_back(
            make_unique<Hiker>(id, name, contact, emergencyContact, startingCheckpoint)
        );

        cout << "\n[OK] Registered Hiker \"" << name << "\" (ID: " << id
             << ") starting at " << startingCheckpoint << ".\n";
        return true;
    }

    void updateHikerLocation(const string& hikerId, const string& newCheckpoint) {
        Hiker* hiker = findHikerById(hikerId);
        if (hiker == nullptr) {
            cout << "\n[FAILED] No hiker found with ID \"" << hikerId << "\".\n";
            return;
        }
        if (!trailMap.hasCheckpoint(newCheckpoint)) {
            cout << "\n[FAILED] \"" << newCheckpoint
                 << "\" is not a known checkpoint on the map.\n";
            return;
        }

        hiker->setCurrentCheckpoint(newCheckpoint);
        cout << "\n[OK] " << hiker->getName() << " is now at " << newCheckpoint << ".\n";
    }

    void updateHikerContact(const string& hikerId, const string& newContact,
                            const string& newEmergency) {
        Hiker* hiker = findHikerById(hikerId);
        if (hiker == nullptr) {
            cout << "\n[FAILED] No hiker found with ID \"" << hikerId << "\".\n";
            return;
        }
        hiker->setContact(newContact);            // ignored if blank
        hiker->setEmergencyContact(newEmergency); // ignored if blank
        cout << "\n[OK] Contact details updated for " << hiker->getName() << ".\n";
    }

    // ---------------- TRAIL MANAGEMENT  (Vidarbh) ----------------

    void addTrailConnection(const string& from, const string& to,
                            double distanceKm, bool bidirectional = true) {
        // Only claim success if the graph actually accepted the trail.
        if (trailMap.addTrail(from, to, distanceKm, bidirectional)) {
            cout << "\n[OK] Trail established: " << from
                 << (bidirectional ? " <---> " : " ---> ") << to
                 << " (" << distanceKm << " km)\n";
        } else {
            cout << "\n[FAILED] Trail was not added to the map.\n";
        }
    }

    void findRouteBetween(const string& from, const string& to) const {
        if (!trailMap.hasCheckpoint(from)) {
            cout << "\n[FAILED] Unknown checkpoint: \"" << from << "\".\n";
            return;
        }
        if (!trailMap.hasCheckpoint(to)) {
            cout << "\n[FAILED] Unknown checkpoint: \"" << to << "\".\n";
            return;
        }

        PathResult route = trailMap.findShortestPath(from, to);

        cout << "\n--- SHORTEST ROUTE (DIJKSTRA) ---\n";
        if (!route.found) {
            cout << "  No trail connects \"" << from << "\" to \"" << to << "\".\n";
            cout << "  (These checkpoints are in separate parts of the network.)\n";
            return;
        }
        displayRoute(route);
    }

    // ---------------- RESCUE DISPATCH  (Shorya) ----------------

    void registerRescueTeam(const RescueTeam& team) {
        trailMap.addCheckpoint(team.getBaseLocation());
        rescueTeams.push_back(team);
    }

    // Evaluates shortest paths from all available rescue bases and dispatches the closest team
    void dispatchNearestTeam(const string& hikerId) {
        Hiker* hiker = findHikerById(hikerId);
        if (hiker == nullptr) {
            cout << "\n[FAILED] No hiker found with ID \"" << hikerId << "\".\n";
            return;
        }

        string hikerLocation = hiker->getCurrentCheckpoint();

        cout << "\n=============================================================\n";
        cout << "            *** SOS EMERGENCY ALERT RECEIVED ***             \n";
        cout << "=============================================================\n";
        cout << "  Hiker    : " << hiker->getName() << " (" << hiker->getId() << ")\n";
        cout << "  Location : " << hikerLocation << "\n";
        cout << "  Emergency contact: " << hiker->getEmergencyContact() << "\n";
        cout << "-------------------------------------------------------------\n";

        int bestTeamIndex = -1;
        double bestDistance = 0.0;
        PathResult bestRoute;

        cout << "  Evaluating available rescue teams...\n\n";

        for (size_t i = 0; i < rescueTeams.size(); ++i) {
            if (!rescueTeams[i].getAvailability()) {
                cout << "    - " << rescueTeams[i].getTeamId()
                     << " : unavailable (already dispatched)\n";
                continue;
            }

            PathResult route = trailMap.findShortestPath(
                rescueTeams[i].getBaseLocation(), hikerLocation);

            if (!route.found) {
                cout << "    - " << rescueTeams[i].getTeamId()
                     << " : no trail route from " << rescueTeams[i].getBaseLocation() << "\n";
                continue;
            }

            cout << "    - " << rescueTeams[i].getTeamId() << " : "
                 << fixed << setprecision(2) << route.totalDistance
                 << " km away\n";
            cout.unsetf(ios::fixed);

            if (bestTeamIndex == -1 || route.totalDistance < bestDistance) {
                bestTeamIndex = static_cast<int>(i);
                bestDistance = route.totalDistance;
                bestRoute = route;
            }
        }

        cout << "-------------------------------------------------------------\n";

        if (bestTeamIndex == -1) {
            cout << "  [!] NO TEAM CAN BE DISPATCHED.\n";
            cout << "  Either every team is busy, or no trail reaches the hiker.\n";
            return;
        }

        RescueTeam& chosen = rescueTeams[bestTeamIndex];
        chosen.setAvailability(false);     // team is now committed

        cout << "  >>> DISPATCHING " << chosen.getTeamId() << " <<<\n";
        cout << "  Led by : " << chosen.getLeader() << "\n";
        cout << "  From   : " << chosen.getBaseLocation() << "\n\n";
        displayRoute(bestRoute);
        cout << "=============================================================\n";
    }

    void recallTeam(const string& teamId) {
        for (size_t i = 0; i < rescueTeams.size(); ++i) {
            if (rescueTeams[i].getTeamId() == teamId) {
                if (rescueTeams[i].getAvailability()) {
                    cout << "\n[INFO] " << teamId << " is already at base and available.\n";
                } else {
                    rescueTeams[i].setAvailability(true);
                    cout << "\n[OK] " << teamId << " has returned to base and is available again.\n";
                }
                return;
            }
        }
        cout << "\n[FAILED] No rescue team with ID \"" << teamId << "\".\n";
    }

    // ---------------- REPORTING  (Rajat) ----------------

    void displaySystemStatus() const {
        cout << "\n=============================================================\n";
        cout << "          TRAIL SAFETY & RESCUE SYSTEM STATUS REPORT         \n";
        cout << "=============================================================\n";

        cout << "\n[SECTION 1: REGISTERED HIKERS] (Total: "
             << registeredUsers.size() << ")\n";
        if (registeredUsers.empty()) {
            cout << "  No hikers registered yet.\n";
        } else {
            for (size_t i = 0; i < registeredUsers.size(); ++i) {
                cout << "\n-- Record #" << (i + 1) << " ["
                     << registeredUsers[i]->getRole() << "] --\n";
                registeredUsers[i]->displayDetails();
            }
        }

        cout << "\n[SECTION 2: RESCUE & RANGER STATIONS] (Total: "
             << rescueTeams.size() << ")\n";
        if (rescueTeams.empty()) {
            cout << "  No rescue teams registered.\n";
        } else {
            for (size_t i = 0; i < rescueTeams.size(); ++i) {
                cout << "\n-- Response Unit #" << (i + 1) << " --\n";
                rescueTeams[i].displayTeam();
            }
        }

        cout << "\n[SECTION 3: TRAIL NETWORK] (Checkpoints: "
             << trailMap.getCheckpointCount() << ")\n";
        trailMap.displayMap();
    }

    // Pre-populates sample trail network and active units for demonstration
    void seedInitialDemoData() {
        // Dehradun & Mussoorie foothill trail network
        trailMap.addTrail("Old Rajpur", "Shikhar Falls", 3.2);
        trailMap.addTrail("Old Rajpur", "Jharipani", 6.5);
        trailMap.addTrail("Jharipani", "Mussoorie Library Chowk", 5.5);
        trailMap.addTrail("Old Rajpur", "Robber's Cave", 6.0);
        trailMap.addTrail("Robber's Cave", "Kimadi Village", 5.5);
        trailMap.addTrail("Kimadi Village", "Hathipaon", 6.2);
        trailMap.addTrail("Mussoorie Library Chowk", "Hathipaon", 4.0);
        trailMap.addTrail("Hathipaon", "George Everest Peak", 2.5);
        trailMap.addTrail("Hathipaon", "Bhadraj Temple", 8.0);
        trailMap.addTrail("George Everest Peak", "Benog Tibba", 4.2);

        // Isolated checkpoint to test unreachable route handling
        trailMap.addCheckpoint("Nag Tibba Base");

        registerRescueTeam(RescueTeam("RT-SDRF-01", "Inspector Rajesh Rawat", "Old Rajpur"));
        registerRescueTeam(RescueTeam("RT-QRT-02", "Officer Sunita Thapa", "Hathipaon"));

        registeredUsers.push_back(make_unique<Hiker>(
            "H-101", "Aarav Sharma", "+91-98765-43210",
            "+91-98765-99887", "George Everest Peak"));
        registeredUsers.push_back(make_unique<Hiker>(
            "H-102", "Meera Joshi", "+91-91234-56780",
            "+91-91234-11223", "Jharipani"));
    }
};


// =============================================================================
//  SECTION D  -  MENU / INPUT HANDLING
//  Owner: RAJAT PANT
// =============================================================================

void clearCin() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Reads a positive floating-point number with input validation
bool readPositiveDouble(double& value) {
    while (true) {
        if (cin >> value) {
            clearCin();
            if (value > 0.0) return true;
            cout << "[!] Distance must be greater than 0. Try again: ";
        } else {
            if (cin.eof()) return false;
            cout << "[!] Please enter a valid number: ";
            clearCin();
        }
    }
}

void printMenu() {
    cout << "\n======================== MAIN MENU ========================\n";
    cout << "  --- Phase 1 : Setup ---\n";
    cout << "  1. Register a Hiker\n";
    cout << "  2. Add a Trail to the Map (Graph Edge)\n";
    cout << "  3. View Full System Status\n";
    cout << "  --- Phase 2 : Routing & Rescue ---\n";
    cout << "  4. Find Shortest Route Between Checkpoints (Dijkstra)\n";
    cout << "  5. Update a Hiker's Current Location\n";
    cout << "  6. Update a Hiker's Contact Details\n";
    cout << "  7. SOS - Dispatch Nearest Rescue Team\n";
    cout << "  8. Recall a Rescue Team to Base\n";
    cout << "  9. Exit\n";
    cout << "===========================================================\n";
    cout << "Enter selection (1-9): ";
}

int main() {
    TrailSafetySystem safetySystem;
    safetySystem.seedInitialDemoData();

    cout << "=============================================================\n";
    cout << "    TRAIL SAFETY & RESCUE MANAGEMENT SYSTEM (PHASE 2)        \n";
    cout << "   B.Tech 3rd Semester PBL - Graph, Dijkstra & OOP           \n";
    cout << "=============================================================\n";
    cout << "[System]: Demo trail network and rescue units loaded.\n";

    int choice = 0;

    while (true) {
        printMenu();

        if (!(cin >> choice)) {
            // Handle stream termination / EOF cleanly
            if (cin.eof()) {
                cout << "\n[System]: Input closed. Shutting down.\n";
                return 0;
            }
            cout << "\n[!] Invalid input. Please enter a number between 1 and 9.\n";
            clearCin();
            continue;
        }
        clearCin();

        switch (choice) {

            case 1: {   // Register a hiker
                string id, name, contact, emergency, start;

                cout << "\n--- REGISTER NEW HIKER ---\n";
                cout << "Hiker ID (e.g. H-103)   : ";  getline(cin, id);
                cout << "Full Name               : ";  getline(cin, name);
                cout << "Contact Number          : ";  getline(cin, contact);
                cout << "Emergency Contact       : ";  getline(cin, emergency);
                cout << "Starting Checkpoint     : ";  getline(cin, start);

                if (start.empty()) start = "Old Rajpur";

                safetySystem.registerHiker(id, name, contact, emergency, start);
                break;
            }

            case 2: {   // Add a trail (graph edge)
                string from, to, biStr;
                double distance = 0.0;

                cout << "\n--- ADD TRAIL CONNECTION (GRAPH EDGE) ---\n";
                cout << "Origin Checkpoint       : ";  getline(cin, from);
                cout << "Destination Checkpoint  : ";  getline(cin, to);
                cout << "Trail Distance (km)     : ";

                if (!readPositiveDouble(distance)) {
                    cout << "\n[System]: Input closed. Shutting down.\n";
                    return 0;
                }

                cout << "Two-way trail? (y/n)    : ";  getline(cin, biStr);
                bool bidirectional = !(!biStr.empty() && (biStr[0] == 'n' || biStr[0] == 'N'));

                safetySystem.addTrailConnection(from, to, distance, bidirectional);
                break;
            }

            case 3: {
                safetySystem.displaySystemStatus();
                break;
            }

            case 4: {   // Dijkstra between any two checkpoints
                string from, to;
                cout << "\n--- FIND SHORTEST ROUTE ---\n";
                cout << "Start Checkpoint        : ";  getline(cin, from);
                cout << "Destination Checkpoint  : ";  getline(cin, to);

                safetySystem.findRouteBetween(from, to);
                break;
            }

            case 5: {   // Move a hiker along the trail
                string id, checkpoint;
                cout << "\n--- UPDATE HIKER LOCATION ---\n";
                cout << "Hiker ID                : ";  getline(cin, id);
                cout << "New Checkpoint          : ";  getline(cin, checkpoint);

                safetySystem.updateHikerLocation(id, checkpoint);
                break;
            }

            case 6: {   // Update contact details
                string id, contact, emergency;
                cout << "\n--- UPDATE HIKER CONTACT DETAILS ---\n";
                cout << "Hiker ID                     : ";  getline(cin, id);
                cout << "New Contact (blank = keep)   : ";  getline(cin, contact);
                cout << "New Emergency (blank = keep) : ";  getline(cin, emergency);

                safetySystem.updateHikerContact(id, contact, emergency);
                break;
            }

            case 7: {   // SOS dispatch
                string id;
                cout << "\n--- SOS EMERGENCY DISPATCH ---\n";
                cout << "Hiker ID in distress    : ";  getline(cin, id);

                safetySystem.dispatchNearestTeam(id);
                break;
            }

            case 8: {   // Free a team up again
                string teamId;
                cout << "\n--- RECALL RESCUE TEAM ---\n";
                cout << "Team ID (e.g. RT-QRT-02): ";  getline(cin, teamId);

                safetySystem.recallTeam(teamId);
                break;
            }

            case 9: {
                cout << "\nExiting Trail Safety and Rescue System. Stay safe!\n";
                return 0;
            }

            default:
                cout << "\n[!] Selection out of range. Choose between 1 and 9.\n";
                break;
        }
    }

    return 0;
}
