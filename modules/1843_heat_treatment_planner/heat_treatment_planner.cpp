#include "heat_treatment_planner.hpp"

namespace heat_treatment_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Heat Treatment Planner (Traditional crafts: horology, blacksmithing, jewelry, glassblowing, pottery, leatherwork, cobbling). Awaits wire-up.";
    return s;
}

}
