#include "photogrammetry_and_splat_importer.hpp"

namespace photogrammetry_and_splat_importer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Photogrammetry and Splat Importer (VR / AR / XR). Awaits wire-up.";
    return s;
}

}
