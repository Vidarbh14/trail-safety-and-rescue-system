// =============================================================================
//  SECTION A  -  ENTITY LAYER (HIKER TELEMETRY & TRACKING)
//  Author: Arnav Negi (Roll No. G/16)
// =============================================================================

#ifndef HIKER_HPP
#define HIKER_HPP

#include "User.hpp"
#include "Common.hpp"
#include <iostream>
#include <vector>
#include <iomanip>

class Hiker : public User {
private:
    std::string emergencyContact;
    std::string medicalNotes;
    std::string currentCheckpoint;
    std::string targetDestination;
    bool trekActive;
    std::vector<std::string> plannedRoute;
    size_t routeStepIndex;
    double totalDistanceCovered;
    std::vector<CheckpointCrossing> travelHistory;

public:
    Hiker(std::string id, std::string personName, std::string contact,
          std::string emergency, std::string startCheckpoint, std::string medical = "None")
        : User(std::move(id), std::move(personName), std::move(contact)),
          emergencyContact(std::move(emergency)),
          medicalNotes(std::move(medical)),
          currentCheckpoint(std::move(startCheckpoint)),
          targetDestination(""),
          trekActive(false),
          routeStepIndex(0),
          totalDistanceCovered(0.0) {
        // Record initial starting checkpoint in history
        travelHistory.push_back(
            CheckpointCrossing(currentCheckpoint, getCurrentTimestamp(), 0.0)
        );
    }

    std::string getRole() const override { return "Hiker"; }

    void displayDetails() const override {
        std::cout << "  [Hiker ID]         : " << userId << "\n"
                  << "  Full Name          : " << name << "\n"
                  << "  Contact Phone      : " << contactNumber << "\n"
                  << "  Emergency Contact  : " << emergencyContact << "\n"
                  << "  Medical & Gear     : " << medicalNotes << "\n"
                  << "  Current Location   : " << currentCheckpoint << "\n"
                  << "  Trek Status        : " << (trekActive ? ("EN ROUTE to " + targetDestination) : "Stationary / Idle") << "\n"
                  << "  Distance Covered   : " << std::fixed << std::setprecision(2) << totalDistanceCovered << " km\n"
                  << "  Checkpoints Crossed: " << travelHistory.size() << "\n";
        std::cout.unsetf(std::ios::fixed);
    }

    std::string getEmergencyContact() const { return emergencyContact; }
    std::string getMedicalNotes() const { return medicalNotes; }
    std::string getCurrentCheckpoint() const { return currentCheckpoint; }
    std::string getTargetDestination() const { return targetDestination; }
    bool isTrekActive() const { return trekActive; }
    const std::vector<std::string>& getPlannedRoute() const { return plannedRoute; }
    size_t getRouteStepIndex() const { return routeStepIndex; }
    double getTotalDistanceCovered() const { return totalDistanceCovered; }
    const std::vector<CheckpointCrossing>& getTravelHistory() const { return travelHistory; }

    void setEmergencyContact(const std::string& contact) {
        if (!contact.empty()) emergencyContact = contact;
    }

    void setMedicalNotes(const std::string& notes) {
        if (!notes.empty()) medicalNotes = notes;
    }

    void setCurrentCheckpoint(const std::string& checkpoint) {
        if (!checkpoint.empty()) currentCheckpoint = checkpoint;
    }

    void startTrek(const std::vector<std::string>& route) {
        if (route.size() < 2) return;
        plannedRoute = route;
        targetDestination = route.back();
        routeStepIndex = 0;
        trekActive = true;
    }

    void finishTrek() {
        trekActive = false;
        targetDestination = "";
        plannedRoute.clear();
        routeStepIndex = 0;
    }

    bool crossNextCheckpoint(double legDistanceKm, std::string& crossedName) {
        if (!trekActive || routeStepIndex + 1 >= plannedRoute.size()) {
            crossedName = "";
            return false;
        }

        routeStepIndex++;
        currentCheckpoint = plannedRoute[routeStepIndex];
        totalDistanceCovered += legDistanceKm;
        crossedName = currentCheckpoint;

        travelHistory.push_back(
            CheckpointCrossing(currentCheckpoint, getCurrentTimestamp(), totalDistanceCovered)
        );

        if (routeStepIndex + 1 >= plannedRoute.size()) {
            trekActive = false;
        }
        return true;
    }

    void displayTravelHistory() const {
        std::cout << "\n=============================================================\n";
        std::cout << "  AUDIT LOG: CHECKPOINT CROSSINGS FOR " << name << " (" << userId << ")\n";
        std::cout << "=============================================================\n";
        if (travelHistory.empty()) {
            std::cout << "  No checkpoint crossing events logged.\n";
            return;
        }
        std::cout << std::left << std::setw(6) << "Step"
                  << std::setw(26) << "Checkpoint Crossed"
                  << std::setw(22) << "Timestamp"
                  << "Total Odometer\n";
        std::cout << std::string(68, '-') << "\n";
        for (size_t i = 0; i < travelHistory.size(); ++i) {
            std::cout << std::left << std::setw(6) << (i + 1)
                      << std::setw(26) << travelHistory[i].checkpointName
                      << std::setw(22) << travelHistory[i].timestamp
                      << std::fixed << std::setprecision(2) << travelHistory[i].cumulativeDistanceKm << " km\n";
        }
        std::cout.unsetf(std::ios::fixed);
        std::cout << std::string(68, '-') << "\n";
    }
};

#endif // HIKER_HPP
