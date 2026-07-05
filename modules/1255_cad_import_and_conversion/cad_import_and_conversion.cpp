#include "cad_import_and_conversion.hpp"

namespace cad_import_and_conversion {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: CAD import and conversion (Robotics and 3D). Awaits wire-up.";
    return s;
}

}
