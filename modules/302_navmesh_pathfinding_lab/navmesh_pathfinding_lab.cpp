#include "navmesh_pathfinding_lab.hpp"

namespace navmesh_pathfinding_lab {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Navmesh & Pathfinding Lab (Game development). Awaits wire-up.";
    return s;
}

}
