// SPDX-License-Identifier: GPL-3.0-or-later
#include "mesh_reconstruction_lab.hpp"

namespace mesh_reconstruction_lab {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Mesh reconstruction lab (Paleontology and archaeology). Awaits wire-up.";
    return s;
}

}
