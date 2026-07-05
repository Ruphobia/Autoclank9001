#include "photogrammetry_and_scan_to_cad.hpp"

namespace photogrammetry_and_scan_to_cad {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Photogrammetry and scan-to-CAD (Mechanical and manufacturing). Awaits wire-up.";
    return s;
}

}
