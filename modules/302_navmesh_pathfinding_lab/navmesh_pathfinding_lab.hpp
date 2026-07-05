#pragma once

#include <string>

// Stub module for the "Navmesh & Pathfinding Lab" tool entry.
// Game development.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace navmesh_pathfinding_lab {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
