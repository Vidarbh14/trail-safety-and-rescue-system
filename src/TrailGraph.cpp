// =============================================================================
//  SECTION B  -  GRAPH IMPLEMENTATION & DIJKSTRA
//  Author: Vidarbh Dabral (Roll No. ML2/67)
// =============================================================================

#include "TrailGraph.hpp"
#include <iostream>
#include <iomanip>
#include <queue>
#include <algorithm>

using namespace std;

string trailStatusToString(TrailStatus status) {
    switch (status) {
        case TrailStatus::OPEN:    return "OPEN";
        case TrailStatus::CAUTION: return "CAUTION (Rough/Weather)";
        case TrailStatus::CLOSED:  return "CLOSED (Impassable)";
    }
    return "UNKNOWN";
}

void TrailGraph::addCheckpoint(const string& checkpointName) {
    if (checkpointName.empty()) return;
    if (adjList.find(checkpointName) == adjList.end()) {
        adjList[checkpointName] = vector<TrailEdge>();
    }
}

bool TrailGraph::addTrail(const string& from, const string& to, double distanceKm,
                          bool bidirectional, TrailStatus status,
                          const string& hazard, double risk) {

    if (from.empty() || to.empty()) {
        cout << "  [!] Error: Checkpoint names cannot be empty.\n";
        return false;
    }
    if (distanceKm <= 0.0) {
        cout << "  [!] Error: Distance must be strictly greater than 0 km.\n";
        return false;
    }
    if (from == to) {
        cout << "  [!] Error: Self-loops not permitted.\n";
        return false;
    }
    if (trailExists(from, to)) {
        cout << "  [!] Error: Trail between \"" << from << "\" and \"" << to << "\" already exists.\n";
        return false;
    }

    addCheckpoint(from);
    addCheckpoint(to);

    adjList[from].push_back(TrailEdge(to, distanceKm, status, hazard, risk));
    if (bidirectional) {
        adjList[to].push_back(TrailEdge(from, distanceKm, status, hazard, risk));
    }
    return true;
}

bool TrailGraph::trailExists(const string& from, const string& to) const {
    auto it = adjList.find(from);
    if (it == adjList.end()) return false;
    for (const auto& edge : it->second) {
        if (edge.destination == to) return true;
    }
    return false;
}

TrailEdge* TrailGraph::getEdgeMutable(const string& from, const string& to) {
    auto it = adjList.find(from);
    if (it == adjList.end()) return nullptr;
    for (auto& edge : it->second) {
        if (edge.destination == to) return &edge;
    }
    return nullptr;
}

const TrailEdge* TrailGraph::getEdge(const string& from, const string& to) const {
    auto it = adjList.find(from);
    if (it == adjList.end()) return nullptr;
    for (const auto& edge : it->second) {
        if (edge.destination == to) return &edge;
    }
    return nullptr;
}

bool TrailGraph::updateTrailStatus(const string& from, const string& to, TrailStatus status,
                                   const string& hazardNotes, double riskMultiplier) {
    TrailEdge* forward = getEdgeMutable(from, to);
    TrailEdge* backward = getEdgeMutable(to, from);

    if (!forward && !backward) return false;

    if (forward) {
        forward->status = status;
        if (!hazardNotes.empty()) forward->hazardNotes = hazardNotes;
        forward->riskMultiplier = riskMultiplier;
    }
    if (backward) {
        backward->status = status;
        if (!hazardNotes.empty()) backward->hazardNotes = hazardNotes;
        backward->riskMultiplier = riskMultiplier;
    }
    return true;
}

bool TrailGraph::hasCheckpoint(const string& name) const {
    return adjList.find(name) != adjList.end();
}

size_t TrailGraph::getCheckpointCount() const {
    return adjList.size();
}

const unordered_map<string, vector<TrailEdge>>& TrailGraph::getAdjList() const {
    return adjList;
}

const vector<TrailEdge>& TrailGraph::getNeighbors(const string& checkpoint) const {
    static const vector<TrailEdge> noTrails;
    auto it = adjList.find(checkpoint);
    if (it == adjList.end()) return noTrails;
    return it->second;
}

// DIJKSTRA'S SHORTEST PATH ALGORITHM
// Time Complexity: O((V + E) log V) using Min-Heap
PathResult TrailGraph::findShortestPath(const string& source, const string& destination,
                                       bool avoidClosedTrails) const {
    PathResult result;

    if (!hasCheckpoint(source) || !hasCheckpoint(destination)) {
        return result;
    }

    unordered_map<string, double> dist;
    unordered_map<string, double> trueDist;
    unordered_map<string, string> previous;
    unordered_map<string, bool> settled;

    for (const auto& entry : adjList) {
        dist[entry.first] = INF;
        trueDist[entry.first] = INF;
    }
    dist[source] = 0.0;
    trueDist[source] = 0.0;

    priority_queue<DistNode, vector<DistNode>, greater<DistNode>> pq;
    pq.push(make_pair(0.0, source));

    while (!pq.empty()) {
        string current = pq.top().second;
        pq.pop();

        if (settled[current]) continue;
        settled[current] = true;

        if (current == destination) break;

        const vector<TrailEdge>& trails = getNeighbors(current);
        for (const auto& trail : trails) {
            if (avoidClosedTrails && trail.status == TrailStatus::CLOSED) {
                continue;
            }

            double weight = trail.distanceKm * trail.riskMultiplier;
            if (trail.status == TrailStatus::CLOSED) {
                weight = INF / 2.0;
            }

            double newEffectiveDist = dist[current] + weight;

            if (newEffectiveDist < dist[trail.destination]) {
                dist[trail.destination] = newEffectiveDist;
                trueDist[trail.destination] = trueDist[current] + trail.distanceKm;
                previous[trail.destination] = current;
                pq.push(make_pair(newEffectiveDist, trail.destination));
            }
        }
    }

    if (dist[destination] >= INF / 2.0) {
        return result;
    }

    vector<string> route;
    string step = destination;
    route.push_back(step);

    while (step != source) {
        step = previous[step];
        route.push_back(step);
    }

    reverse(route.begin(), route.end());

    result.found = true;
    result.totalDistance = trueDist[destination];
    result.effectiveRiskDistance = dist[destination];
    result.path = route;

    for (size_t i = 0; i + 1 < route.size(); ++i) {
        const TrailEdge* edge = getEdge(route[i], route[i + 1]);
        if (edge && edge->status == TrailStatus::CAUTION) {
            result.safetyWarnings.push_back(
                "Caution on trail " + route[i] + " -> " + route[i + 1] + ": " + edge->hazardNotes
            );
        }
    }

    return result;
}

void TrailGraph::displayMap() const {
    if (adjList.empty()) {
        cout << "  (Network is currently empty.)\n";
        return;
    }

    cout << "\n" << left << setw(24) << "Checkpoint (Node)"
         << "Connected Trails [Distance, Status, Hazard]\n";
    cout << string(75, '-') << "\n";

    for (const auto& entry : adjList) {
        cout << "  * " << left << setw(22) << entry.first << " -> ";
        if (entry.second.empty()) {
            cout << "[Isolated node - no outbound trails]";
        } else {
            for (size_t i = 0; i < entry.second.size(); ++i) {
                const auto& e = entry.second[i];
                string tag = (e.status == TrailStatus::OPEN ? "OPEN" :
                             (e.status == TrailStatus::CAUTION ? "CAUTION" : "CLOSED"));
                cout << e.destination << " (" << fixed << setprecision(1) << e.distanceKm
                     << " km | " << tag << ")";
                if (i + 1 < entry.second.size()) cout << ", ";
            }
        }
        cout << "\n";
    }
    cout.unsetf(ios::fixed);
    cout << string(75, '-') << "\n";
}
