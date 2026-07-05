#include "redistricting_plan_editor.hpp"

namespace redistricting_plan_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Redistricting plan editor (Sociology, political science, public policy). Awaits wire-up.";
    return s;
}

}
