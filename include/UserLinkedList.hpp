// =============================================================================
//  SECTION A  -  CUSTOM DATA STRUCTURE: USER LINKED LIST
//  Author: Arnav Negi (Roll No. G/16)
//
//  Data Structure:
//    - Singly Linked List with head & tail pointers for O(1) appending.
//    - Manual heap allocation (Node* with pointers) from 3rd Sem DSA syllabus.
//    - Destructor traversal ensures RAII clean teardown without memory leaks.
// =============================================================================

#ifndef USER_LINKED_LIST_HPP
#define USER_LINKED_LIST_HPP

#include "User.hpp"
#include "Hiker.hpp"
#include <memory>
#include <string>

// Node structure containing a polymorphic User pointer and a pointer to the next node
struct UserNode {
    std::unique_ptr<User> user;
    UserNode* next;

    UserNode(std::unique_ptr<User> u) : user(std::move(u)), next(nullptr) {}
};

// Custom Singly Linked List managing dynamic User nodes in memory
class UserLinkedList {
private:
    UserNode* head;
    UserNode* tail;
    size_t count;

public:
    UserLinkedList() : head(nullptr), tail(nullptr), count(0) {}

    // Destructor: Cleanly traverses the list and frees every node
    ~UserLinkedList() {
        clear();
    }

    // Disable copy semantics to prevent shallow pointer duplicates
    UserLinkedList(const UserLinkedList&) = delete;
    UserLinkedList& operator=(const UserLinkedList&) = delete;

    void clear() {
        while (head) {
            UserNode* temp = head;
            head = head->next;
            delete temp;
        }
        tail = nullptr;
        count = 0;
    }

    // O(1) Insertion at tail of linked list
    void append(std::unique_ptr<User> u) {
        UserNode* newNode = new UserNode(std::move(u));
        if (!head) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        count++;
    }

    // Linear Search traversal: O(N) by User ID
    Hiker* findHikerById(const std::string& id) const {
        UserNode* curr = head;
        while (curr) {
            if (curr->user && curr->user->getId() == id) {
                return dynamic_cast<Hiker*>(curr->user.get());
            }
            curr = curr->next;
        }
        return nullptr;
    }

    UserNode* getHead() const { return head; }
    size_t size() const { return count; }
    bool empty() const { return count == 0; }
};

#endif // USER_LINKED_LIST_HPP
