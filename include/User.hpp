// =============================================================================
//  SECTION A  -  ENTITY LAYER (OOP ARCHITECTURE)
//  Author: Arnav Negi (Roll No. G/16)
// =============================================================================

#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <utility>

// Structure representing an audit record of a checkpoint crossed by a hiker
struct CheckpointCrossing {
    std::string checkpointName;
    std::string timestamp;
    double cumulativeDistanceKm;

    CheckpointCrossing(std::string name, std::string time, double dist)
        : checkpointName(std::move(name)), timestamp(std::move(time)), cumulativeDistanceKm(dist) {}
};

// Abstract Base Class: Defines common interface and attributes for users
class User {
protected:
    std::string userId;
    std::string name;
    std::string contactNumber;

public:
    virtual ~User() = default;

    User(std::string id, std::string personName, std::string contact)
        : userId(std::move(id)), name(std::move(personName)), contactNumber(std::move(contact)) {}

    // Pure virtual functions ensuring derived classes implement role-specific behavior
    virtual void displayDetails() const = 0;
    virtual std::string getRole() const = 0;

    // Const-correct getters
    std::string getId() const { return userId; }
    std::string getName() const { return name; }
    std::string getContact() const { return contactNumber; }

    // Mutators with validation
    void setName(const std::string& newName) {
        if (!newName.empty()) name = newName;
    }

    void setContact(const std::string& newContact) {
        if (!newContact.empty()) contactNumber = newContact;
    }
};

#endif // USER_HPP
