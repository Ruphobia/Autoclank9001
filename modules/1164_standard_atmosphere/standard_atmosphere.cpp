#include "standard_atmosphere.hpp"

namespace standard_atmosphere {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Standard Atmosphere (Aerodynamics and CFD). Awaits wire-up.";
    return s;
}

}
