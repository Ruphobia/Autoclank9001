#include "tle_workbench.hpp"

namespace tle_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: TLE Workbench (Aerospace and orbital mechanics). Awaits wire-up.";
    return s;
}

}
