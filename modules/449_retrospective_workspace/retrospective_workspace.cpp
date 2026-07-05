#include "retrospective_workspace.hpp"

namespace retrospective_workspace {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Retrospective workspace (Project management). Awaits wire-up.";
    return s;
}

}
