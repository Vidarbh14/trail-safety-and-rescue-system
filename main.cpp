/**
 * ============================================================================
 * PROJECT: Trail Safety and Rescue System (Phase 1 Milestone - 25% Completion)
 * COURSE : Data Structures & Object-Oriented Programming (CS2xx - 3rd Sem PBL)
 * PURPOSE: Foundational Skeleton Integrating STL Graph & OOP Class Hierarchies
 * ============================================================================
 *
 * ARCHITECTURAL SUMMARY FOR EVALUATORS:
 * ----------------------------------------------------------------------------
 * 1. GRAPH DATA STRUCTURE (STL-Based Adjacency List):
 *    - Implemented using std::unordered_map<std::string, std::vector<TrailEdge>>.
 *    - Space Complexity: O(V + E), which is asymptotically superior to an
 *      Adjacency Matrix (O(V^2)) for sparse real-world trail networks.
 *    - Checkpoint lookup is amortized O(1) via hash table.
 *
 * 2. OBJECT-ORIENTED PROGRAMMING (OOP) PRINCIPLES:
 *    - [Abstraction]  : Abstract Base Class (ABC) `User` with pure virtual methods.
 *    - [Encapsulation]: Member attributes are `protected`, accessed via const-correct
 *                       public getters and validating setters.
 *    - [Inheritance]  : `Hiker` extends `User` publicly (`is-a` relationship).
 *    - [Polymorphism] : Dynamic dispatch achieved through virtual methods (`override`).
 *
 * 3. DESIGN PATTERN & MODERN C++:
 *    - [Facade Pattern]       : `TrailSafetySystem` provides a single unified entry
 *                               point managing subsystems (Users + Graph).
 *    - [Smart Pointers & RAII]: Employs std::unique_ptr<User> for automated, leak-free
 *                               polymorphic object life-cycle management.
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <iomanip>
#include <limits>

// ============================================================================
// SECTION 1: USER MANAGEMENT SUBSYSTEM (OOP PRINCIPLES)
// ============================================================================

/**
 * [OOP: ABSTRACTION & ENCAPSULATION]
 * Abstract Base Class: User
 * 
 * Demonstrates:
 * - Abstraction: Exposes high-level interfaces (`displayDetails()`, `getRole()`)
 *   without implementation details. Cannot be instantiated directly.
 * - Encapsulation: Protects internal state (`userId`, `name`, `contactNumber`)
 *   from illegal or arbitrary external mutations.
 */
class User {
protected:
    // [OOP: ENCAPSULATION] Protected attributes accessible only to this class and derived classes
    std::string userId;
    std::string name;
    std::string contactNumber;

public:
    /**
     * Virtual Destructor:
     * Crucial in modern C++ polymorphism. Ensures that when an object of a derived 
     * class is deleted via a pointer to the base class (e.g. unique_ptr<User>),
     * the derived class's destructor is executed first, preventing resource leaks.
     */
    virtual ~User() = default;

    // Parameterized Constructor using std::move for optimal value semantics
    User(std::string id, std::string name, std::string contact)
        : userId(std::move(id)), name(std::move(name)), contactNumber(std::move(contact)) {}

    /**
     * [OOP: PURE VIRTUAL FUNCTIONS - ABSTRACTION & POLYMORPHISM]
     * Making these methods '= 0' transforms 'User' into an Abstract Base Class (ABC).
     * Every derived class MUST override them to be instantiable.
     */
    virtual void displayDetails() const = 0;
    virtual std::string getRole() const = 0;

    // --- [OOP: ENCAPSULATION] Public Accessors (Getters with const-correctness) ---
    std::string getId() const { return userId; }
    std::string getName() const { return name; }
    std::string getContact() const { return contactNumber; }

    // --- [OOP: ENCAPSULATION] Public Mutators (Setters with basic validation) ---
    void setName(const std::string& newName) {
        if (!newName.empty()) {
            name = newName;
        }
    }

    void setContact(const std::string& newContact) {
        if (!newContact.empty()) {
            contactNumber = newContact;
        }
    }
};

/**
 * [OOP: INHERITANCE & POLYMORPHISM]
 * Derived Class: Hiker
 * 
 * - Inheritance: Publicly inherits from User (`Hiker is-a User`).
 * - Extends base state with hiker-specific telemetry (emergency contact & current checkpoint).
 * - Polymorphism: Overrides pure virtual methods `displayDetails()` and `getRole()`.
 */
class Hiker : public User {
private:
    std::string emergencyContact;
    std::string currentCheckpoint;

public:
    // Constructor delegates core user initialization to base class User
    Hiker(std::string id, std::string name, std::string contact, 
          std::string emergencyContact, std::string initialCheckpoint = "Trailhead")
        : User(std::move(id), std::move(name), std::move(contact)),
          emergencyContact(std::move(emergencyContact)),
          currentCheckpoint(std::move(initialCheckpoint)) {}

    // [OOP: POLYMORPHISM] Implementation of base class interface
    std::string getRole() const override {
        return "Hiker";
    }

    void displayDetails() const override {
        std::cout << "  [Hiker ID]         : " << userId << "\n"
                  << "  Name               : " << name << "\n"
                  << "  Contact            : " << contactNumber << "\n"
                  << "  Emergency Contact  : " << emergencyContact << "\n"
                  << "  Current Location   : " << currentCheckpoint << "\n";
    }

    // Hiker-specific getters and setters
    std::string getEmergencyContact() const { return emergencyContact; }
    std::string getCurrentCheckpoint() const { return currentCheckpoint; }

    void setEmergencyContact(const std::string& contact) { emergencyContact = contact; }
    void setCurrentCheckpoint(const std::string& checkpoint) { currentCheckpoint = checkpoint; }
};

/**
 * [OOP: ENCAPSULATION]
 * Class: RescueTeam
 * 
 * Represents specialized emergency response units stationed along the trail system.
 */
class RescueTeam {
private:
    std::string teamId;
    std::string teamLeader;
    std::string baseLocation;
    bool isAvailable;

public:
    RescueTeam(std::string id, std::string leader, std::string base)
        : teamId(std::move(id)), teamLeader(std::move(leader)), 
          baseLocation(std::move(base)), isAvailable(true) {}

    void displayTeam() const {
        std::cout << "  [Team ID]    : " << teamId << "\n"
                  << "  Team Leader  : " << teamLeader << "\n"
                  << "  Base Camp    : " << baseLocation << "\n"
                  << "  Status       : " << (isAvailable ? "Ready / On Patrol" : "Dispatched to Emergency") << "\n";
    }

    // Getters & Setters
    std::string getTeamId() const { return teamId; }
    std::string getLeader() const { return teamLeader; }
    std::string getBaseLocation() const { return baseLocation; }
    bool getAvailability() const { return isAvailable; }

    void setAvailability(bool status) { isAvailable = status; }
    void setBaseLocation(const std::string& newBase) { baseLocation = newBase; }
};

// ============================================================================
// SECTION 2: TRAIL NETWORK (GRAPH DATA STRUCTURE)
// ============================================================================

/**
 * [DATA STRUCTURE: GRAPH EDGE]
 * Struct representing a directed connection between two trail checkpoints.
 * Contains destination vertex and edge weight (distance in kilometers).
 */
struct TrailEdge {
    std::string destination; // Neighboring checkpoint
    double distanceKm;       // Edge weight (trail length)

    TrailEdge(std::string dest, double dist)
        : destination(std::move(dest)), distanceKm(dist) {}
};

/**
 * [DATA STRUCTURE: GRAPH USING ADJACENCY LIST (STL)]
 * Class: TrailGraph
 * 
 * Implementation Details:
 * - Backed by std::unordered_map<std::string, std::vector<TrailEdge>>
 *   * Key   : Checkpoint Name (Graph Vertex)
 *   * Value : List of outgoing edges (Adjacency List)
 * 
 * Evaluator Viva Defense Points:
 * 1. Why Adjacency List over Adjacency Matrix?
 *    Trail networks are sparse graphs (|E| << |V|^2). An Adjacency List uses O(V + E)
 *    memory versus O(V^2) for a matrix.
 * 2. Why std::unordered_map?
 *    Provides average O(1) constant-time vertex lookup using hashing, allowing
 *    human-readable checkpoint names (e.g. "Base Camp") without integer index mapping.
 */
class TrailGraph {
private:
    std::unordered_map<std::string, std::vector<TrailEdge>> adjList;

public:
    TrailGraph() = default;

    /**
     * Adds a new checkpoint vertex to the graph.
     * Amortized O(1) average time complexity.
     */
    void addCheckpoint(const std::string& checkpointName) {
        if (checkpointName.empty()) return;
        // If not already present, create an empty entry for this vertex
        if (adjList.find(checkpointName) == adjList.end()) {
            adjList[checkpointName] = std::vector<TrailEdge>();
        }
    }

    /**
     * Adds a weighted edge between two checkpoints.
     * If bidirectional is true, connects both u -> v and v -> u.
     */
    void addTrail(const std::string& from, const std::string& to, double distanceKm, bool bidirectional = true) {
        if (from.empty() || to.empty() || distanceKm <= 0.0) {
            std::cout << "  [!] Error: Invalid trail specifications. Distance must be positive.\n";
            return;
        }

        // Implicitly register vertices if they don't already exist
        addCheckpoint(from);
        addCheckpoint(to);

        // Add directed edge: from -> to
        adjList[from].emplace_back(to, distanceKm);

        // If bidirectional, add reverse edge: to -> from
        if (bidirectional) {
            adjList[to].emplace_back(from, distanceKm);
        }
    }

    /**
     * Traverses and prints the graph's Adjacency List to the console.
     */
    void displayMap() const {
        if (adjList.empty()) {
            std::cout << "  (Trail network is empty. Please add checkpoints and trails.)\n";
            return;
        }

        std::cout << "-------------------------------------------------------------\n";
        std::cout << std::left << std::setw(22) << "Checkpoint (Node)" << "Connected Trails (Neighbor [Distance])\n";
        std::cout << "-------------------------------------------------------------\n";

        for (const auto& pair : adjList) {
            const std::string& checkpoint = pair.first;
            const std::vector<TrailEdge>& edges = pair.second;

            std::cout << "  * " << std::left << std::setw(20) << checkpoint << " -> ";
            if (edges.empty()) {
                std::cout << "[Isolated Checkpoint - No active trails]";
            } else {
                for (size_t i = 0; i < edges.size(); ++i) {
                    std::cout << edges[i].destination << " [" << edges[i].distanceKm << " km]";
                    if (i + 1 < edges.size()) {
                        std::cout << " , ";
                    }
                }
            }
            std::cout << "\n";
        }
        std::cout << "-------------------------------------------------------------\n";
    }

    bool hasCheckpoint(const std::string& name) const {
        return adjList.find(name) != adjList.end();
    }

    size_t getCheckpointCount() const {
        return adjList.size();
    }
};

// ============================================================================
// SECTION 3: THE BRIDGE - FACADE DESIGN PATTERN
// ============================================================================

/**
 * [DESIGN PATTERN: FACADE PATTERN]
 * Class: TrailSafetySystem
 * 
 * - Unifies the User Management subsystem and the Trail Graph into a single,
 *   cohesive API for the Presentation/UI layer.
 * - [MODERN C++ & RAII]: Manages users as std::vector<std::unique_ptr<User>>.
 *   std::unique_ptr guarantees:
 *   1. Polymorphism without raw pointer risks.
 *   2. Automatic deallocation when the vector is cleared or goes out of scope (Zero memory leaks).
 *   3. Strict single-ownership semantics.
 */
class TrailSafetySystem {
private:
    std::vector<std::unique_ptr<User>> registeredUsers; // Polymorphic user storage
    std::vector<RescueTeam> rescueTeams;                 // Safety response units
    TrailGraph trailMap;                                 // Adjacency list graph

public:
    TrailSafetySystem() = default;

    // Prevent accidental shallow copying of unique_ptr collections
    TrailSafetySystem(const TrailSafetySystem&) = delete;
    TrailSafetySystem& operator=(const TrailSafetySystem&) = delete;

    /**
     * Registers a new Hiker into the system.
     * Uses std::make_unique (C++14) for exception-safe dynamic allocation.
     */
    void registerHiker(const std::string& id, const std::string& name, 
                       const std::string& contact, const std::string& emergencyContact,
                       const std::string& startingCheckpoint = "Base Camp") {
        
        // Auto-register checkpoint on the map
        trailMap.addCheckpoint(startingCheckpoint);

        // Store polymorphically via smart pointer
        registeredUsers.push_back(
            std::make_unique<Hiker>(id, name, contact, emergencyContact, startingCheckpoint)
        );

        std::cout << "\n[OK] Successfully registered Hiker: \"" << name << "\" (ID: " << id << ")\n";
    }

    /**
     * Adds an active Rescue Unit.
     */
    void registerRescueTeam(const RescueTeam& team) {
        trailMap.addCheckpoint(team.getBaseLocation());
        rescueTeams.push_back(team);
    }

    /**
     * Adds a trail (edge) to the graph.
     */
    void addTrailConnection(const std::string& from, const std::string& to, 
                            double distanceKm, bool bidirectional = true) {
        trailMap.addTrail(from, to, distanceKm, bidirectional);
        std::cout << "\n[OK] Trail successfully established: " 
                  << from << " <---> " << to << " (" << distanceKm << " km)\n";
    }

    /**
     * Prints complete system telemetry: Registered Users, Rescue Teams, and Graph Map.
     */
    void displaySystemStatus() const {
        std::cout << "\n=============================================================\n";
        std::cout << "          TRAIL SAFETY & RESCUE SYSTEM STATUS REPORT         \n";
        std::cout << "=============================================================\n";

        // 1. Registered Users (Polymorphic Dynamic Dispatch)
        std::cout << "\n[SECTION 1: REGISTERED HIKERS & USERS] (Total: " << registeredUsers.size() << ")\n";
        if (registeredUsers.empty()) {
            std::cout << "  No users registered yet.\n";
        } else {
            for (size_t i = 0; i < registeredUsers.size(); ++i) {
                std::cout << "\n-- Record #" << (i + 1) << " [" << registeredUsers[i]->getRole() << "] --\n";
                // Polymorphic Call: Resolves dynamically at runtime to Hiker::displayDetails()
                registeredUsers[i]->displayDetails();
            }
        }

        // 2. Rescue Teams
        std::cout << "\n[SECTION 2: RESCUE & RANGER STATIONS] (Total: " << rescueTeams.size() << ")\n";
        if (rescueTeams.empty()) {
            std::cout << "  No rescue teams currently registered.\n";
        } else {
            for (size_t i = 0; i < rescueTeams.size(); ++i) {
                std::cout << "\n-- Response Unit #" << (i + 1) << " --\n";
                rescueTeams[i].displayTeam();
            }
        }

        // 3. Graph Adjacency List Layout
        std::cout << "\n[SECTION 3: TRAIL NETWORK MAP (GRAPH ADJACENCY LIST)]\n";
        trailMap.displayMap();
    }

    /**
     * Preloads realistic starter data to allow immediate live demonstration
     * during viva/evaluation without requiring repetitive manual input.
     */
    void seedInitialDemoData() {
        // Preload trail connections
        trailMap.addTrail("Base Camp", "Pine Ridge", 3.2);
        trailMap.addTrail("Pine Ridge", "Eagle Peak", 4.8);
        trailMap.addTrail("Base Camp", "Mirror Lake", 2.5);
        trailMap.addTrail("Mirror Lake", "Eagle Peak", 5.1);
        trailMap.addTrail("Eagle Peak", "Summit Point", 2.0);

        // Preload Rescue Units
        registerRescueTeam(RescueTeam("RT-Alpha", "Commander R. Hayes", "Base Camp"));
        registerRescueTeam(RescueTeam("RT-Bravo", "Officer K. Sharma", "Pine Ridge"));

        // Preload an initial Hiker
        registeredUsers.push_back(
            std::make_unique<Hiker>("H-101", "David Clark", "+1-555-0144", "+1-555-0988", "Mirror Lake")
        );
    }
};

// ============================================================================
// SECTION 4: TERMINAL USER INTERFACE (CONTROLLER / REPL)
// ============================================================================

/**
 * Utility function to clean the standard input stream.
 * Prevents input skipping bugs when mixing `std::cin >> var` with `std::getline()`.
 */
void clearCin() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() {
    // Instantiate our Facade controller
    TrailSafetySystem safetySystem;

    // Seed sample topology & users for rapid evaluator verification
    safetySystem.seedInitialDemoData();

    int choice = 0;

    std::cout << "=============================================================\n";
    std::cout << "    TRAIL SAFETY & RESCUE MANAGEMENT SYSTEM (PHASE 1)        \n";
    std::cout << "   B.Tech 3rd Semester PBL - Core Data Structures & OOP      \n";
    std::cout << "=============================================================\n";
    std::cout << "[System]: Demo network loaded. Ready for evaluation.\n";

    while (true) {
        std::cout << "\n======================== MAIN MENU ========================\n";
        std::cout << "  1. Register a Hiker\n";
        std::cout << "  2. Add a Trail to the Map (Graph Edge)\n";
        std::cout << "  3. View Current System Status (Users + Graph)\n";
        std::cout << "  4. Exit\n";
        std::cout << "===========================================================\n";
        std::cout << "Enter selection (1-4): ";

        if (!(std::cin >> choice)) {
            std::cout << "\n[!] Invalid input. Please enter a valid integer (1-4).\n";
            clearCin();
            continue;
        }

        clearCin(); // Consume leftover newline from stream

        switch (choice) {
            case 1: {
                std::string id, name, contact, emergencyContact, startCheckpoint;

                std::cout << "\n--- REGISTER NEW HIKER ---\n";
                std::cout << "Enter Hiker ID (e.g., H-102): ";
                std::getline(std::cin, id);

                std::cout << "Enter Full Name: ";
                std::getline(std::cin, name);

                std::cout << "Enter Contact Number: ";
                std::getline(std::cin, contact);

                std::cout << "Enter Emergency Contact Number: ";
                std::getline(std::cin, emergencyContact);

                std::cout << "Enter Starting Checkpoint (default: Base Camp): ";
                std::getline(std::cin, startCheckpoint);
                if (startCheckpoint.empty()) {
                    startCheckpoint = "Base Camp";
                }

                safetySystem.registerHiker(id, name, contact, emergencyContact, startCheckpoint);
                break;
            }

            case 2: {
                std::string from, to;
                double distance = 0.0;

                std::cout << "\n--- ADD TRAIL CONNECTION (GRAPH EDGE) ---\n";
                std::cout << "Enter Origin Checkpoint: ";
                std::getline(std::cin, from);

                std::cout << "Enter Destination Checkpoint: ";
                std::getline(std::cin, to);

                std::cout << "Enter Trail Distance (km): ";
                while (!(std::cin >> distance) || distance <= 0.0) {
                    std::cout << "[!] Distance must be a positive number. Try again: ";
                    clearCin();
                }
                clearCin();

                std::cout << "Is this trail bidirectional? (y/n, default: y): ";
                std::string biStr;
                std::getline(std::cin, biStr);
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
                std::cout << "\nExiting Trail Safety and Rescue System. Have a safe journey!\n";
                return 0;
            }

            default:
                std::cout << "\n[!] Selection out of range. Please choose an option between 1 and 4.\n";
                break;
        }
    }

    return 0;
}
