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
#include <utility>

// Custom Linked List Node for Hiker Checkpoint Crossing Log
struct CrossingNode {
    CheckpointCrossing crossing;
    CrossingNode* next;

    CrossingNode(CheckpointCrossing c) : crossing(std::move(c)), next(nullptr) {}
};

// Custom Singly Linked List for hiker telemetry audit logs
class CrossingLinkedList {
private:
    CrossingNode* head;
    CrossingNode* tail;
    size_t count;

public:
    CrossingLinkedList() : head(nullptr), tail(nullptr), count(0) {}

    ~CrossingLinkedList() {
        clear();
    }

    CrossingLinkedList(const CrossingLinkedList& other) : head(nullptr), tail(nullptr), count(0) {
        CrossingNode* curr = other.head;
        while (curr) {
            append(curr->crossing);
            curr = curr->next;
        }
    }

    CrossingLinkedList& operator=(const CrossingLinkedList& other) {
        if (this != &other) {
            clear();
            CrossingNode* curr = other.head;
            while (curr) {
                append(curr->crossing);
                curr = curr->next;
            }
        }
        return *this;
    }

    void clear() {
        while (head) {
            CrossingNode* temp = head;
            head = head->next;
            delete temp;
        }
        tail = nullptr;
        count = 0;
    }

    void append(const CheckpointCrossing& c) {
        CrossingNode* newNode = new CrossingNode(c);
        if (!head) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        count++;
    }

    CrossingNode* getHead() const { return head; }
    size_t size() const { return count; }
    bool empty() const { return count == 0; }
};

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
    CrossingLinkedList travelHistory;

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
        // Record initial starting checkpoint into custom linked list
        travelHistory.append(
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
    const CrossingLinkedList& getTravelHistory() const { return travelHistory; }

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

        travelHistory.append(
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
        CrossingNode* curr = travelHistory.getHead();
        int step = 1;
        while (curr) {
            std::cout << std::left << std::setw(6) << step++
                      << std::setw(26) << curr->crossing.checkpointName
                      << std::setw(22) << curr->crossing.timestamp
                      << std::fixed << std::setprecision(2) << curr->crossing.cumulativeDistanceKm << " km\n";
            curr = curr->next;
        }
        std::cout.unsetf(std::ios::fixed);
        std::cout << std::string(68, '-') << "\n";
    }
};

#endif // HIKER_HPP
