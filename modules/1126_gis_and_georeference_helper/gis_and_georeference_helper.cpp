#include "gis_and_georeference_helper.hpp"

namespace gis_and_georeference_helper {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: GIS and georeference helper (Heavy engineering: naval, petroleum, mining, nuclear). Awaits wire-up.";
    return s;
}

}
