// SPDX-License-Identifier: GPL-3.0-or-later
#include "plasmid_map_editor.hpp"

namespace plasmid_map_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Plasmid Map Editor (Biology and bioinformatics). Awaits wire-up.";
    return s;
}

}
