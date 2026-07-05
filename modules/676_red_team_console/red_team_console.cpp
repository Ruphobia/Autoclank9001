#include "red_team_console.hpp"

namespace red_team_console {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Red-team console (Data, statistics, and ML). Awaits wire-up.";
    return s;
}

}
