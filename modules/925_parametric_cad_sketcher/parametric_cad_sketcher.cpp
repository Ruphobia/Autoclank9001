#include "parametric_cad_sketcher.hpp"

namespace parametric_cad_sketcher {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Parametric CAD sketcher (Mechanical and manufacturing). Awaits wire-up.";
    return s;
}

}
