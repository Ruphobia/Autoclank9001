#include "route_notebook.hpp"

namespace route_notebook {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Route Notebook (Esports, speedrunning, competitive game analytics). Awaits wire-up.";
    return s;
}

}
