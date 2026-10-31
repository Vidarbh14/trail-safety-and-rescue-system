// =============================================================================
//  SECTION A  -  ENTITY LAYER (RESCUE TEAM MANAGEMENT)
//  Author: Arnav Negi (Roll No. G/16)
// =============================================================================

#ifndef RESCUE_TEAM_HPP
#define RESCUE_TEAM_HPP

#include <string>
#include <iostream>
#include <utility>

class RescueTeam {
private:
    std::string teamId;
    std::string teamLeader;
    std::string baseLocation;
    bool isAvailable;
    std::string currentMission;

public:
    RescueTeam(std::string id, std::string leader, std::string base)
        : teamId(std::move(id)), teamLeader(std::move(leader)),
          baseLocation(std::move(base)), isAvailable(true), currentMission("On Patrol / Standby") {}

    void displayTeam() const {
        std::cout << "  [Team ID]    : " << teamId << "\n"
                  << "  Commander    : " << teamLeader << "\n"
                  << "  Base Camp    : " << baseLocation << "\n"
                  << "  Status       : "
                  << (isAvailable ? "Ready for Dispatch" : ("DISPATCHED (" + currentMission + ")")) << "\n";
    }

    std::string getTeamId() const { return teamId; }
    std::string getLeader() const { return teamLeader; }
    std::string getBaseLocation() const { return baseLocation; }
    bool getAvailability() const { return isAvailable; }
    std::string getMission() const { return currentMission; }

    void setAvailability(bool status, const std::string& mission = "On Patrol / Standby") {
        isAvailable = status;
        currentMission = mission;
    }
};

#endif // RESCUE_TEAM_HPP
