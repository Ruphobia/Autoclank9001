#include "dig_site_gis.hpp"

namespace dig_site_gis {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Dig site GIS (Paleontology and archaeology). Awaits wire-up.";
    return s;
}

}
