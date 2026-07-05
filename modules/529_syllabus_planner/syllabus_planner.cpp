#include "syllabus_planner.hpp"

namespace syllabus_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Syllabus Planner (Education and pedagogy). Awaits wire-up.";
    return s;
}

}
