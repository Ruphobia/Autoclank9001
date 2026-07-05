#include "body_size_and_mass_estimator.hpp"

namespace body_size_and_mass_estimator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Body size and mass estimator (Paleontology and archaeology). Awaits wire-up.";
    return s;
}

}
