#include "sonar_log_viewer.hpp"

namespace sonar_log_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sonar log viewer (Oceanography and marine science). Awaits wire-up.";
    return s;
}

}
