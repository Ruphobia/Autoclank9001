#include "lesson_plan_composer.hpp"

namespace lesson_plan_composer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Lesson Plan Composer (Education and pedagogy). Awaits wire-up.";
    return s;
}

}
