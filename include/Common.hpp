#ifndef COMMON_HPP
#define COMMON_HPP

#include <string>
#include <chrono>
#include <ctime>
#include <limits>

#ifdef _WIN32
#include <windows.h>
inline void sleepMs(int ms) { Sleep(ms); }
#else
#include <unistd.h>
inline void sleepMs(int ms) { usleep(ms * 1000); }
#endif

// Representation of infinity for unvisited graph nodes in Dijkstra
const double INF = std::numeric_limits<double>::max();

// Helper function: returns formatted local timestamp
inline std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    time_t now_c = std::chrono::system_clock::to_time_t(now);
    tm* local_tm = localtime(&now_c);
    char buf[64];
    if (local_tm) {
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", local_tm);
    } else {
        return "2026-10-29 00:00:00";
    }
    return std::string(buf);
}

#endif // COMMON_HPP
