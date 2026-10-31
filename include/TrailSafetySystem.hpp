// =============================================================================
//  SECTION C  -  FACADE SYSTEM, SOS DISPATCH, TRACKER & PERSISTENCE
//  Author: Shorya Negi (Roll No. 221106)
// =============================================================================

#ifndef TRAIL_SAFETY_SYSTEM_HPP
#define TRAIL_SAFETY_SYSTEM_HPP

#include "User.hpp"
#include "Hiker.hpp"
#include "RescueTeam.hpp"
#include "TrailGraph.hpp"
#include "UserLinkedList.hpp"
#include <vector>
#include <memory>
#include <string>

class TrailSafetySystem {
private:
    // Core Data Structures:
    // 1. Custom Singly Linked List for registered users (Arnav Negi)
    // 2. Vector for active rescue units (Arnav Negi)
    // 3. Adjacency List Graph for checkpoints & trails (Vidarbh Dabral)
    UserLinkedList registeredUsers;
    std::vector<RescueTeam> rescueTeams;
    TrailGraph trailMap;

    void displayRoute(const PathResult& route) const;

public:
    TrailSafetySystem() = default;
    TrailSafetySystem(const TrailSafetySystem&) = delete;
    TrailSafetySystem& operator=(const TrailSafetySystem&) = delete;

    // Hiker Management
    Hiker* findHikerById(const std::string& id) const;

    bool registerHiker(const std::string& id, const std::string& name,
                       const std::string& contact, const std::string& emergencyContact,
                       const std::string& startingCheckpoint, const std::string& medical = "None");

    void updateHikerLocation(const std::string& hikerId, const std::string& newCheckpoint);

    void updateHikerContact(const std::string& hikerId, const std::string& newContact,
                            const std::string& newEmergency);

    // Automatic Location Tracking
    bool startHikerTrek(const std::string& hikerId, const std::string& destination);
    bool advanceHikerSingleCheckpoint(const std::string& hikerId);
    void runAutoTrackingSimulation(const std::string& hikerId);
    void displayHikerCrossingLog(const std::string& hikerId) const;

    // Trail Management & Hazards
    void addTrailConnection(const std::string& from, const std::string& to, double distanceKm,
                            bool bidirectional = true);
    void findRouteBetween(const std::string& from, const std::string& to) const;
    void setTrailHazardCondition(const std::string& from, const std::string& to, TrailStatus status,
                                 const std::string& hazardNotes, double riskMultiplier);
    void broadcastEmergencyAlert(const std::string& message) const;

    // SOS Dispatch
    void registerRescueTeam(const RescueTeam& team);
    void dispatchNearestTeam(const std::string& hikerId);
    void recallTeam(const std::string& teamId);

    // Persistence & Web Export
    bool saveSystemState(const std::string& filename = "trail_safety_data.txt") const;
    bool loadSystemState(const std::string& filename = "trail_safety_data.txt");
    bool exportStateToJson(const std::string& filename = "trail_network.json") const;

    void displaySystemStatus() const;
    void seedInitialDemoData();
};

#endif // TRAIL_SAFETY_SYSTEM_HPP
