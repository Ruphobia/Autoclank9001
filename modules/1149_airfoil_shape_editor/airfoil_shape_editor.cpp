#include "airfoil_shape_editor.hpp"

namespace airfoil_shape_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Airfoil Shape Editor (Aerodynamics and CFD). Awaits wire-up.";
    return s;
}

}
