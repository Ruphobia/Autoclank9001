#include "material_library.hpp"

namespace material_library {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Material library (Mechanical and manufacturing). Awaits wire-up.";
    return s;
}

}
