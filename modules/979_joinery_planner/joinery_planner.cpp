#include "joinery_planner.hpp"

namespace joinery_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Joinery Planner (Maker fab and personal manufacturing). Awaits wire-up.";
    return s;
}

}
