#include "site_plan_tool.hpp"

namespace site_plan_tool {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Site Plan Tool (Civil, architectural, and HVAC engineering). Awaits wire-up.";
    return s;
}

}
