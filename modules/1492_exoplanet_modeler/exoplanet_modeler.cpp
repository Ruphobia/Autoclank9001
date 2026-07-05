#include "exoplanet_modeler.hpp"

namespace exoplanet_modeler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Exoplanet Modeler (Astronomy and astrophysics). Awaits wire-up.";
    return s;
}

}
