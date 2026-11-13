// =============================================================================
//  SECTION B  -  GRAPH DATA STRUCTURE & DIJKSTRA'S ALGORITHM
//  Author: Vidarbh Dabral (Roll No. ML2/67)
// =============================================================================

#ifndef TRAIL_GRAPH_HPP
#define TRAIL_GRAPH_HPP

#include "Common.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>

enum class TrailStatus {
    OPEN,
    CAUTION,
    CLOSED
};

std::string trailStatusToString(TrailStatus status);

struct TrailEdge {
    std::string destination;
    double distanceKm;
    TrailStatus status;
    std::string hazardNotes;
    double riskMultiplier;

    TrailEdge(std::string dest, double dist, TrailStatus stat = TrailStatus::OPEN,
              std::string hazard = "Clear trail", double risk = 1.0)
        : destination(std::move(dest)), distanceKm(dist), status(stat),
          hazardNotes(std::move(hazard)), riskMultiplier(risk) {}
};

struct PathResult {
    bool found;
    double totalDistance;
    double effectiveRiskDistance;
    std::vector<std::string> path;
    std::vector<std::string> safetyWarnings;

    PathResult() : found(false), totalDistance(0.0), effectiveRiskDistance(0.0) {}
};

using DistNode = std::pair<double, std::string>;

class TrailGraph {
private:
    std::unordered_map<std::string, std::vector<TrailEdge>> adjList;

public:
    TrailGraph() = default;

    void addCheckpoint(const std::string& checkpointName);

    bool addTrail(const std::string& from, const std::string& to, double distanceKm,
                  bool bidirectional = true, TrailStatus status = TrailStatus::OPEN,
                  const std::string& hazard = "Clear trail", double risk = 1.0);

    bool trailExists(const std::string& from, const std::string& to) const;

    TrailEdge* getEdgeMutable(const std::string& from, const std::string& to);
    const TrailEdge* getEdge(const std::string& from, const std::string& to) const;

    bool updateTrailStatus(const std::string& from, const std::string& to, TrailStatus status,
                           const std::string& hazardNotes = "", double riskMultiplier = 1.0);

    bool hasCheckpoint(const std::string& name) const;
    size_t getCheckpointCount() const;
    size_t getTotalTrailCount() const;

    const std::unordered_map<std::string, std::vector<TrailEdge>>& getAdjList() const;
    const std::vector<TrailEdge>& getNeighbors(const std::string& checkpoint) const;

    PathResult findShortestPath(const std::string& source, const std::string& destination,
                               bool avoidClosedTrails = true) const;

    void displayMap() const;
};

#endif // TRAIL_GRAPH_HPP
