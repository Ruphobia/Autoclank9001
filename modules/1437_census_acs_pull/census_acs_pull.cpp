#include "census_acs_pull.hpp"

namespace census_acs_pull {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Census/ACS pull (Sociology, political science, public policy). Awaits wire-up.";
    return s;
}

}
