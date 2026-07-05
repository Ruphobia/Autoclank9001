#include "ctd_profile_plotter.hpp"

namespace ctd_profile_plotter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: CTD profile plotter (Oceanography and marine science). Awaits wire-up.";
    return s;
}

}
