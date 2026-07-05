#include "telescope_pointing.hpp"

namespace telescope_pointing {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Telescope Pointing (Astronomy and astrophysics). Awaits wire-up.";
    return s;
}

}
