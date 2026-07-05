// SPDX-License-Identifier: GPL-3.0-or-later
#include "mesh_editor.hpp"

namespace mesh_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Mesh editor (Mechanical and manufacturing). Awaits wire-up.";
    return s;
}

}
