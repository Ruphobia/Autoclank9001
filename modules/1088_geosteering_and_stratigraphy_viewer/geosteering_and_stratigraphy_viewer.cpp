#include "geosteering_and_stratigraphy_viewer.hpp"

namespace geosteering_and_stratigraphy_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Geosteering and stratigraphy viewer (Heavy engineering: naval, petroleum, mining, nuclear). Awaits wire-up.";
    return s;
}

}
