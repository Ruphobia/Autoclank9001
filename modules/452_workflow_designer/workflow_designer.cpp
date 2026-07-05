#include "workflow_designer.hpp"

namespace workflow_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Workflow designer (Project management). Awaits wire-up.";
    return s;
}

}
