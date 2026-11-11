// =============================================================================
//  SECTION A  -  CUSTOM DATA STRUCTURE: RESCUE TEAM LINKED LIST
//  Author: Arnav Negi (Roll No. G/16)
//
//  Data Structure:
//    - Singly Linked List with head & tail pointers for O(1) appending.
//    - Manual heap allocation (RescueTeamNode* with pointers) for DSA curriculum.
//    - Destructor traversal ensures RAII clean teardown without memory leaks.
// =============================================================================

#ifndef RESCUE_TEAM_LINKED_LIST_HPP
#define RESCUE_TEAM_LINKED_LIST_HPP

#include "RescueTeam.hpp"
#include <string>
#include <utility>

// Dynamic node holding a RescueTeam and a pointer to the next node
struct RescueTeamNode {
    RescueTeam team;
    RescueTeamNode* next;

    RescueTeamNode(RescueTeam t) : team(std::move(t)), next(nullptr) {}
};

// Custom Singly Linked List for managing Rescue Teams in memory
class RescueTeamLinkedList {
private:
    RescueTeamNode* head;
    RescueTeamNode* tail;
    size_t count;

public:
    RescueTeamLinkedList() : head(nullptr), tail(nullptr), count(0) {}

    ~RescueTeamLinkedList() {
        clear();
    }

    RescueTeamLinkedList(const RescueTeamLinkedList&) = delete;
    RescueTeamLinkedList& operator=(const RescueTeamLinkedList&) = delete;

    void clear() {
        while (head) {
            RescueTeamNode* temp = head;
            head = head->next;
            delete temp;
        }
        tail = nullptr;
        count = 0;
    }

    // O(1) Insertion at tail
    void append(const RescueTeam& team) {
        RescueTeamNode* newNode = new RescueTeamNode(team);
        if (!head) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        count++;
    }

    // Linear search traversal by Team ID
    RescueTeam* findTeamById(const std::string& id) {
        RescueTeamNode* curr = head;
        while (curr) {
            if (curr->team.getTeamId() == id) {
                return &(curr->team);
            }
            curr = curr->next;
        }
        return nullptr;
    }

    const RescueTeam* findTeamById(const std::string& id) const {
        RescueTeamNode* curr = head;
        while (curr) {
            if (curr->team.getTeamId() == id) {
                return &(curr->team);
            }
            curr = curr->next;
        }
        return nullptr;
    }

    RescueTeamNode* getHead() const { return head; }
    size_t size() const { return count; }
    bool empty() const { return count == 0; }
};

#endif // RESCUE_TEAM_LINKED_LIST_HPP
