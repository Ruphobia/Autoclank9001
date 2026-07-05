#include "microdata_explorer.hpp"

namespace microdata_explorer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Microdata explorer (Sociology, political science, public policy). Awaits wire-up.";
    return s;
}

}
