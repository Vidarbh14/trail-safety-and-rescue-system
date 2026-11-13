// =============================================================================
//  SECTION C  -  FACADE SYSTEM, SOS DISPATCH, TRACKER & PERSISTENCE
//  Author: Shorya Negi (Roll No. 221106)
// =============================================================================

#include "TrailSafetySystem.hpp"
#include "UI.hpp"
#include "Common.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

using namespace std;

void TrailSafetySystem::displayRoute(const PathResult& route) const {
    cout << "  Route      : ";
    for (size_t i = 0; i < route.path.size(); ++i) {
        cout << route.path[i];
        if (i + 1 < route.path.size()) cout << " -> ";
    }
    cout << "\n";
    cout << "  Total Dist : " << fixed << setprecision(2) << route.totalDistance << " km\n";
    cout << "  Checkpoints: " << route.path.size() << " checkpoints along path\n";
    if (!route.safetyWarnings.empty()) {
        cout << "  [!] ROUTE WARNINGS:\n";
        for (const auto& w : route.safetyWarnings) {
            cout << "      - " << w << "\n";
        }
    }
    cout.unsetf(ios::fixed);
}

// Linked List linear search O(N) traversal
Hiker* TrailSafetySystem::findHikerById(const string& id) const {
    return registeredUsers.findHikerById(id);
}

bool TrailSafetySystem::registerHiker(const string& id, const string& name,
                                     const string& contact, const string& emergencyContact,
                                     const string& startingCheckpoint, const string& medical) {

    if (id.empty() || name.empty()) {
        cout << "\n[FAILED] Hiker ID and Name are mandatory.\n";
        return false;
    }
    if (findHikerById(id) != nullptr) {
        cout << "\n[FAILED] Hiker with ID \"" << id << "\" is already registered.\n";
        return false;
    }

    trailMap.addCheckpoint(startingCheckpoint);
    
    // Insert new node into custom UserLinkedList
    registeredUsers.append(
        make_unique<Hiker>(id, name, contact, emergencyContact, startingCheckpoint, medical)
    );

    cout << "\n[OK] Registered Hiker \"" << name << "\" (ID: " << id
         << ") into Linked List at checkpoint: " << startingCheckpoint << ".\n";
    return true;
}

void TrailSafetySystem::updateHikerLocation(const string& hikerId, const string& newCheckpoint) {
    Hiker* hiker = findHikerById(hikerId);
    if (hiker == nullptr) {
        cout << "\n[FAILED] No hiker found with ID \"" << hikerId << "\".\n";
        return;
    }
    if (!trailMap.hasCheckpoint(newCheckpoint)) {
        cout << "\n[FAILED] \"" << newCheckpoint << "\" is not a recognized checkpoint.\n";
        return;
    }

    hiker->setCurrentCheckpoint(newCheckpoint);
    cout << "\n[OK] " << hiker->getName() << "'s location directly updated to: " << newCheckpoint << ".\n";
}

void TrailSafetySystem::updateHikerContact(const string& hikerId, const string& newContact,
                                          const string& newEmergency) {
    Hiker* hiker = findHikerById(hikerId);
    if (hiker == nullptr) {
        cout << "\n[FAILED] No hiker found with ID \"" << hikerId << "\".\n";
        return;
    }
    hiker->setContact(newContact);
    hiker->setEmergencyContact(newEmergency);
    cout << "\n[OK] Contact details updated for " << hiker->getName() << ".\n";
}

bool TrailSafetySystem::startHikerTrek(const string& hikerId, const string& destination) {
    Hiker* hiker = findHikerById(hikerId);
    if (!hiker) {
        cout << "\n[FAILED] Hiker not found.\n";
        return false;
    }

    string origin = hiker->getCurrentCheckpoint();
    if (origin == destination) {
        cout << "\n[!] Hiker is already at target destination \"" << destination << "\".\n";
        return false;
    }

    PathResult route = trailMap.findShortestPath(origin, destination, true);
    if (!route.found) {
        cout << "\n[FAILED] No open trail path exists from \""
             << origin << "\" to \"" << destination << "\".\n";
        return false;
    }

    hiker->startTrek(route.path);
    cout << "\n=============================================================\n";
    cout << "  TREK ROUTE REGISTERED & AUTOMATIC TRACKING ACTIVATED       \n";
    cout << "=============================================================\n";
    cout << "  Hiker      : " << hiker->getName() << " (" << hiker->getId() << ")\n";
    cout << "  Start Point: " << origin << "\n";
    cout << "  Destination: " << destination << "\n";
    displayRoute(route);
    cout << "[System]: Geo-fence and RFID sensors ready for tracking.\n";
    return true;
}

bool TrailSafetySystem::advanceHikerSingleCheckpoint(const string& hikerId) {
    Hiker* hiker = findHikerById(hikerId);
    if (!hiker) {
        cout << "\n[FAILED] Hiker not found.\n";
        return false;
    }
    if (!hiker->isTrekActive()) {
        cout << "\n[INFO] Hiker " << hiker->getName() << " does not have an active trek.\n";
        return false;
    }

    size_t currentIdx = hiker->getRouteStepIndex();
    const auto& route = hiker->getPlannedRoute();
    string fromCheckpoint = route[currentIdx];
    string toCheckpoint = route[currentIdx + 1];

    const TrailEdge* edge = trailMap.getEdge(fromCheckpoint, toCheckpoint);
    double legDist = edge ? edge->distanceKm : 1.0;

    if (edge && edge->status == TrailStatus::CLOSED) {
        cout << "\n[SAFETY HALT]: Trail from \"" << fromCheckpoint
             << "\" to \"" << toCheckpoint << "\" is CLOSED due to hazard!\n";
        cout << "[System]: Hiker halted safely at \"" << fromCheckpoint << "\".\n";
        return false;
    }

    string crossedCheckpoint;
    if (hiker->crossNextCheckpoint(legDist, crossedCheckpoint)) {
        cout << "\n-------------------------------------------------------------\n";
        cout << "  [TELEMETRY: CHECKPOINT SENSOR PING RECEIVED]\n";
        cout << "  Timestamp        : " << getCurrentTimestamp() << "\n";
        cout << "  Hiker            : " << hiker->getName() << " (" << hiker->getId() << ")\n";
        cout << "  Checkpoint Passed: >>> " << crossedCheckpoint << " <<<\n";
        cout << "  Leg Distance     : " << fixed << setprecision(2) << legDist << " km\n";
        cout << "  Total Odometer   : " << hiker->getTotalDistanceCovered() << " km\n";

        if (hiker->getCurrentCheckpoint() == hiker->getTargetDestination()) {
            cout << "  Trek Status      : *** ARRIVED SAFELY AT DESTINATION ***\n";
        } else {
            cout << "  Next Target Stop : " << route[hiker->getRouteStepIndex() + 1] << "\n";
        }
        cout.unsetf(ios::fixed);
        cout << "-------------------------------------------------------------\n";
        return true;
    }
    return false;
}

void TrailSafetySystem::runAutoTrackingSimulation(const string& hikerId) {
    Hiker* hiker = findHikerById(hikerId);
    if (!hiker) {
        cout << "\n[FAILED] Hiker not found.\n";
        return;
    }
    if (!hiker->isTrekActive()) {
        cout << "\n[!] Hiker " << hiker->getName() << " has no active trek route assigned.\n";
        cout << "    Please select Menu Option 6 ('Plan & Start Hiker Trek') first.\n";
        return;
    }

    cout << "\n=============================================================\n";
    cout << "  STARTING AUTOMATIC REAL-TIME CHECKPOINT TRACKING           \n";
    cout << "  Hiker: " << hiker->getName() << " | Heading for: " << hiker->getTargetDestination() << "\n";
    cout << "=============================================================\n";

    while (hiker->isTrekActive()) {
        size_t idx = hiker->getRouteStepIndex();
        const auto& route = hiker->getPlannedRoute();
        string from = route[idx];
        string to = route[idx + 1];

        cout << "\n[Sensor Radar]: Tracking movement from \"" << from << "\" -> \"" << to << "\"...\n";

        for (int p = 0; p <= 100; p += 25) {
            renderProgressBar(p, "Walking Trail");
            sleepMs(120);
        }
        cout << "\n";

        bool progressed = advanceHikerSingleCheckpoint(hikerId);
        if (!progressed) {
            cout << "\n[Tracking Paused]: Hazard or halt encountered along route.\n";
            break;
        }
    }

    cout << "\n[Tracking Session Finished]: " << hiker->getName()
         << " is now stationed at: " << hiker->getCurrentCheckpoint() << "\n";
}

void TrailSafetySystem::displayHikerCrossingLog(const string& hikerId) const {
    Hiker* hiker = findHikerById(hikerId);
    if (!hiker) {
        cout << "\n[FAILED] Hiker not found.\n";
        return;
    }
    hiker->displayTravelHistory();
}

void TrailSafetySystem::addTrailConnection(const string& from, const string& to, double distanceKm,
                                          bool bidirectional) {
    if (trailMap.addTrail(from, to, distanceKm, bidirectional)) {
        cout << "\n[OK] Trail connection established: " << from
             << (bidirectional ? " <---> " : " ---> ") << to
             << " (" << distanceKm << " km)\n";
    } else {
        cout << "\n[FAILED] Trail was rejected.\n";
    }
}

void TrailSafetySystem::findRouteBetween(const string& from, const string& to) const {
    if (!trailMap.hasCheckpoint(from) || !trailMap.hasCheckpoint(to)) {
        cout << "\n[FAILED] One or both checkpoints do not exist in map.\n";
        return;
    }

    PathResult route = trailMap.findShortestPath(from, to, true);
    cout << "\n--- SHORTEST SAFE ROUTE (DIJKSTRA) ---\n";
    if (!route.found) {
        cout << "  No open trail connects \"" << from << "\" to \"" << to << "\".\n";
        return;
    }
    displayRoute(route);
}

void TrailSafetySystem::setTrailHazardCondition(const string& from, const string& to, TrailStatus status,
                                               const string& hazardNotes, double riskMultiplier) {
    if (!trailMap.updateTrailStatus(from, to, status, hazardNotes, riskMultiplier)) {
        cout << "\n[FAILED] Trail between \"" << from << "\" and \"" << to << "\" not found.\n";
        return;
    }

    cout << "\n[OK] Trail status updated for (" << from << " <-> " << to << ")\n";
    cout << "     New Status : " << trailStatusToString(status) << "\n";
    cout << "     Hazard Note: " << hazardNotes << "\n";

    // Linked List traversal to alert affected hikers
    cout << "\n[Safety Radar]: Scanning Linked List for affected hikers...\n";
    bool alertTriggered = false;
    UserNode* curr = registeredUsers.getHead();
    while (curr) {
        Hiker* h = dynamic_cast<Hiker*>(curr->user.get());
        if (h) {
            if (h->getCurrentCheckpoint() == from || h->getCurrentCheckpoint() == to) {
                cout << "  >>> URGENT SAFETY ALERT SENT TO " << h->getName()
                     << " (" << h->getContact() << ") located at " << h->getCurrentCheckpoint() << "!\n";
                alertTriggered = true;
            }
        }
        curr = curr->next;
    }
    if (!alertTriggered) {
        cout << "  No registered hikers currently stationed in hazard zone.\n";
    }
}

void TrailSafetySystem::broadcastEmergencyAlert(const string& message) const {
    cout << "\n=============================================================\n";
    cout << "        *** PARK-WIDE EMERGENCY BROADCAST TO ALL HIKERS *** \n";
    cout << "=============================================================\n";
    cout << "  Timestamp : " << getCurrentTimestamp() << "\n";
    cout << "  BROADCAST : " << message << "\n\n";

    if (registeredUsers.empty()) {
        cout << "  (No hikers registered on the network).\n";
    } else {
        UserNode* curr = registeredUsers.getHead();
        while (curr) {
            Hiker* h = dynamic_cast<Hiker*>(curr->user.get());
            if (h) {
                cout << "  [SMS/Radio Alert Delivered] -> " << h->getName()
                     << " (Phone: " << h->getContact() << " | At: "
                     << h->getCurrentCheckpoint() << ")\n";
            }
            curr = curr->next;
        }
    }
    cout << "=============================================================\n";
}

void TrailSafetySystem::registerRescueTeam(const RescueTeam& team) {
    trailMap.addCheckpoint(team.getBaseLocation());
    rescueTeams.append(team);
}

void TrailSafetySystem::dispatchNearestTeam(const string& hikerId) {
    Hiker* hiker = findHikerById(hikerId);
    if (hiker == nullptr) {
        cout << "\n[FAILED] No hiker found with ID \"" << hikerId << "\".\n";
        return;
    }

    string hikerLocation = hiker->getCurrentCheckpoint();

    cout << "\n=============================================================\n";
    cout << "            *** SOS EMERGENCY ALERT RECEIVED ***             \n";
    cout << "=============================================================\n";
    cout << "  Hiker             : " << hiker->getName() << " (" << hiker->getId() << ")\n";
    cout << "  Verified Location : " << hikerLocation << " (Updated via Telemetry)\n";
    cout << "  Emergency Contact : " << hiker->getEmergencyContact() << "\n";
    cout << "  Medical Profile   : " << hiker->getMedicalNotes() << "\n";
    cout << "-------------------------------------------------------------\n";

    RescueTeam* bestTeam = nullptr;
    double bestDistance = INF;
    PathResult bestRoute;

    cout << "  Evaluating available rescue units via Dijkstra...\n\n";

    RescueTeamNode* rNode = rescueTeams.getHead();
    while (rNode) {
        RescueTeam& team = rNode->team;
        if (!team.getAvailability()) {
            cout << "    - " << team.getTeamId() << " : unavailable (on active mission)\n";
            rNode = rNode->next;
            continue;
        }

        PathResult route = trailMap.findShortestPath(
            team.getBaseLocation(), hikerLocation, true);

        if (!route.found) {
            cout << "    - " << team.getTeamId()
                 << " : no open trail from base camp " << team.getBaseLocation() << "\n";
            rNode = rNode->next;
            continue;
        }

        cout << "    - " << team.getTeamId() << " ("
             << team.getBaseLocation() << ") : "
             << fixed << setprecision(2) << route.totalDistance << " km away\n";
        cout.unsetf(ios::fixed);

        if (!bestTeam || route.totalDistance < bestDistance) {
            bestTeam = &team;
            bestDistance = route.totalDistance;
            bestRoute = route;
        }
        rNode = rNode->next;
    }

    cout << "-------------------------------------------------------------\n";

    if (!bestTeam) {
        cout << "  [!] NO RESCUE TEAM CAN BE DISPATCHED.\n";
        cout << "  All teams busy or trails impassable.\n";
        return;
    }

    bestTeam->setAvailability(false, "SOS mission for " + hiker->getName());

    cout << "  >>> DISPATCHING UNIT: " << bestTeam->getTeamId() << " <<<\n";
    cout << "  Commander : " << bestTeam->getLeader() << "\n";
    cout << "  Base Camp : " << bestTeam->getBaseLocation() << "\n";
    cout << "  Target    : " << hikerLocation << "\n\n";
    displayRoute(bestRoute);
    cout << "=============================================================\n";
}

void TrailSafetySystem::recallTeam(const string& teamId) {
    RescueTeam* team = rescueTeams.findTeamById(teamId);
    if (team) {
        if (team->getAvailability()) {
            cout << "\n[INFO] " << teamId << " is already stationed at base camp.\n";
        } else {
            team->setAvailability(true, "On Patrol / Standby");
            cout << "\n[OK] " << teamId << " has returned to base and is available again.\n";
        }
        return;
    }
    cout << "\n[FAILED] No rescue team with ID \"" << teamId << "\".\n";
}

bool TrailSafetySystem::saveSystemState(const string& filename) const {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        cout << "\n[FAILED] Unable to open file for saving: " << filename << "\n";
        return false;
    }

    outFile << "# TRAIL_SAFETY_PERSISTENCE_V1\n";

    outFile << "[TRAILS]\n";
    for (const auto& entry : trailMap.getAdjList()) {
        for (const auto& edge : entry.second) {
            if (entry.first <= edge.destination) {
                outFile << entry.first << "|" << edge.destination << "|"
                        << edge.distanceKm << "|"
                        << static_cast<int>(edge.status) << "|"
                        << edge.hazardNotes << "|"
                        << edge.riskMultiplier << "\n";
            }
        }
    }

    // Traverse Linked List to persist all hikers
    outFile << "[HIKERS]\n";
    UserNode* curr = registeredUsers.getHead();
    while (curr) {
        Hiker* h = dynamic_cast<Hiker*>(curr->user.get());
        if (h) {
            outFile << h->getId() << "|" << h->getName() << "|"
                    << h->getContact() << "|" << h->getEmergencyContact() << "|"
                    << h->getCurrentCheckpoint() << "|" << h->getMedicalNotes() << "\n";
        }
        curr = curr->next;
    }

    outFile << "[RESCUE_TEAMS]\n";
    RescueTeamNode* rNode = rescueTeams.getHead();
    while (rNode) {
        outFile << rNode->team.getTeamId() << "|" << rNode->team.getLeader() << "|"
                << rNode->team.getBaseLocation() << "|" << (rNode->team.getAvailability() ? 1 : 0) << "\n";
        rNode = rNode->next;
    }

    outFile.close();
    cout << "\n[OK] System state saved successfully to \"" << filename << "\".\n";
    return true;
}

bool TrailSafetySystem::loadSystemState(const string& filename) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cout << "\n[FAILED] Unable to open file for reading: " << filename << "\n";
        return false;
    }

    registeredUsers.clear();
    rescueTeams.clear();
    trailMap = TrailGraph();

    string line;
    string currentSection = "";

    while (getline(inFile, line)) {
        if (line.empty() || line[0] == '#') continue;

        if (line == "[TRAILS]" || line == "[HIKERS]" || line == "[RESCUE_TEAMS]") {
            currentSection = line;
            continue;
        }

        stringstream ss(line);
        string token;
        vector<string> parts;
        while (getline(ss, token, '|')) {
            parts.push_back(token);
        }

        if (currentSection == "[TRAILS]" && parts.size() >= 6) {
            string from = parts[0];
            string to = parts[1];
            double dist = stod(parts[2]);
            TrailStatus stat = static_cast<TrailStatus>(stoi(parts[3]));
            string hazard = parts[4];
            double risk = stod(parts[5]);
            trailMap.addTrail(from, to, dist, true, stat, hazard, risk);
        }
        else if (currentSection == "[HIKERS]" && parts.size() >= 6) {
            registerHiker(parts[0], parts[1], parts[2], parts[3], parts[4], parts[5]);
        }
        else if (currentSection == "[RESCUE_TEAMS]" && parts.size() >= 4) {
            RescueTeam team(parts[0], parts[1], parts[2]);
            team.setAvailability(parts[3] == "1");
            registerRescueTeam(team);
        }
    }

    inFile.close();
    cout << "\n[OK] System state loaded successfully from \"" << filename << "\".\n";
    return true;
}

bool TrailSafetySystem::exportStateToJson(const string& filename) const {
    ofstream jsonFile(filename);
    if (!jsonFile.is_open()) {
        cout << "\n[FAILED] Unable to open JSON file for writing: " << filename << "\n";
        return false;
    }

    jsonFile << "{\n";
    jsonFile << "  \"generatedAt\": \"" << getCurrentTimestamp() << "\",\n";

    jsonFile << "  \"trails\": [\n";
    bool firstEdge = true;
    for (const auto& entry : trailMap.getAdjList()) {
        for (const auto& edge : entry.second) {
            if (entry.first <= edge.destination) {
                if (!firstEdge) jsonFile << ",\n";
                jsonFile << "    {\n"
                         << "      \"from\": \"" << entry.first << "\",\n"
                         << "      \"to\": \"" << edge.destination << "\",\n"
                         << "      \"distanceKm\": " << edge.distanceKm << ",\n"
                         << "      \"status\": \"" << trailStatusToString(edge.status) << "\",\n"
                         << "      \"hazard\": \"" << edge.hazardNotes << "\"\n"
                         << "    }";
                firstEdge = false;
            }
        }
    }
    jsonFile << "\n  ],\n";

    // Linked List traversal for JSON export
    jsonFile << "  \"hikers\": [\n";
    UserNode* curr = registeredUsers.getHead();
    bool firstHiker = true;
    while (curr) {
        Hiker* h = dynamic_cast<Hiker*>(curr->user.get());
        if (h) {
            if (!firstHiker) jsonFile << ",\n";
            jsonFile << "    {\n"
                     << "      \"id\": \"" << h->getId() << "\",\n"
                     << "      \"name\": \"" << h->getName() << "\",\n"
                     << "      \"contact\": \"" << h->getContact() << "\",\n"
                     << "      \"currentCheckpoint\": \"" << h->getCurrentCheckpoint() << "\",\n"
                     << "      \"isTrekActive\": " << (h->isTrekActive() ? "true" : "false") << ",\n"
                     << "      \"totalDistanceKm\": " << h->getTotalDistanceCovered() << "\n"
                     << "    }";
            firstHiker = false;
        }
        curr = curr->next;
    }
    jsonFile << "\n  ],\n";

    jsonFile << "  \"rescueTeams\": [\n";
    RescueTeamNode* rNode = rescueTeams.getHead();
    bool firstTeam = true;
    while (rNode) {
        if (!firstTeam) jsonFile << ",\n";
        jsonFile << "    {\n"
                 << "      \"id\": \"" << rNode->team.getTeamId() << "\",\n"
                 << "      \"leader\": \"" << rNode->team.getLeader() << "\",\n"
                 << "      \"base\": \"" << rNode->team.getBaseLocation() << "\",\n"
                 << "      \"isAvailable\": " << (rNode->team.getAvailability() ? "true" : "false") << "\n"
                 << "    }";
        firstTeam = false;
        rNode = rNode->next;
    }
    jsonFile << "\n  ]\n";
    jsonFile << "}\n";

    jsonFile.close();
    cout << "\n[OK] Web dashboard JSON exported to \"" << filename << "\".\n";
    return true;
}

void TrailSafetySystem::displaySystemStatus() const {
    cout << "\n=============================================================\n";
    cout << "       TRAIL SAFETY & MOUNTAIN RESCUE SYSTEM OVERVIEW        \n";
    cout << "=============================================================\n";

    cout << "\n[SECTION 1: REGISTERED HIKERS (CUSTOM LINKED LIST)] (Total: "
         << registeredUsers.size() << ")\n";
    if (registeredUsers.empty()) {
        cout << "  No hikers registered yet.\n";
    } else {
        UserNode* curr = registeredUsers.getHead();
        size_t nodeIndex = 1;
        while (curr) {
            cout << "\n-- Linked List Node #" << nodeIndex++ << " [" << curr->user->getRole() << "] --\n";
            curr->user->displayDetails();
            curr = curr->next;
        }
    }

    cout << "\n[SECTION 2: RESCUE & RANGER STATIONS (CUSTOM LINKED LIST)] (Total: "
         << rescueTeams.size() << " | Available: " << getAvailableRescueTeamCount() << ")\n";
    if (rescueTeams.empty()) {
        cout << "  No rescue teams on duty.\n";
    } else {
        RescueTeamNode* rCurr = rescueTeams.getHead();
        size_t unitNum = 1;
        while (rCurr) {
            cout << "\n-- Linked List Node #" << unitNum++ << " --\n";
            rCurr->team.displayTeam();
            rCurr = rCurr->next;
        }
    }

    cout << "\n[SECTION 3: TRAIL MAP & HAZARDS] (Checkpoints: "
         << trailMap.getCheckpointCount() << ")\n";
    trailMap.displayMap();
}

void TrailSafetySystem::seedInitialDemoData() {
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

    trailMap.addCheckpoint("Nag Tibba Base");

    registerRescueTeam(RescueTeam("RT-SDRF-01", "Inspector Rajesh Rawat", "Old Rajpur"));
    registerRescueTeam(RescueTeam("RT-QRT-02", "Officer Sunita Thapa", "Hathipaon"));

    // Insert into UserLinkedList
    registeredUsers.append(make_unique<Hiker>(
        "H-101", "Aarav Sharma", "+91-98765-43210",
        "+91-98765-99887", "George Everest Peak", "Asthma inhaler in backpack"));
    registeredUsers.append(make_unique<Hiker>(
        "H-102", "Meera Joshi", "+91-91234-56780",
        "+91-91234-11223", "Old Rajpur", "First aid kit equipped"));
}

size_t TrailSafetySystem::getRegisteredHikerCount() const {
    return registeredUsers.size();
}

size_t TrailSafetySystem::getAvailableRescueTeamCount() const {
    size_t readyCount = 0;
    RescueTeamNode* curr = rescueTeams.getHead();
    while (curr) {
        if (curr->team.getAvailability()) {
            readyCount++;
        }
        curr = curr->next;
    }
    return readyCount;
}
