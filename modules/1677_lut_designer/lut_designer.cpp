#include "lut_designer.hpp"

namespace lut_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: LUT Designer (Photography and cinematography). Awaits wire-up.";
    return s;
}

}
