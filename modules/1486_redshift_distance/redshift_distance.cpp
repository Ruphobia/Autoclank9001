#include "redshift_distance.hpp"

namespace redshift_distance {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Redshift Distance (Astronomy and astrophysics). Awaits wire-up.";
    return s;
}

}
