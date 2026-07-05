#include "dive_log_decompression_planner.hpp"

namespace dive_log_decompression_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Dive log + decompression planner (Oceanography and marine science). Awaits wire-up.";
    return s;
}

}
