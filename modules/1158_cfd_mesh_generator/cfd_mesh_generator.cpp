#include "cfd_mesh_generator.hpp"

namespace cfd_mesh_generator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: CFD Mesh Generator (Aerodynamics and CFD). Awaits wire-up.";
    return s;
}

}
