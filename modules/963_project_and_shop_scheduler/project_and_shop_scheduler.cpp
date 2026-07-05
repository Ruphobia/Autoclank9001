#include "project_and_shop_scheduler.hpp"

namespace project_and_shop_scheduler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Project and shop scheduler (Mechanical and manufacturing). Awaits wire-up.";
    return s;
}

}
