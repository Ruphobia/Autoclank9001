#include "step_iges_converter.hpp"

namespace step_iges_converter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: STEP/IGES converter (Mechanical and manufacturing). Awaits wire-up.";
    return s;
}

}
