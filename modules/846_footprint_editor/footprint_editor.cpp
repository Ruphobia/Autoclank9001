// SPDX-License-Identifier: GPL-3.0-or-later
#include "footprint_editor.hpp"

namespace footprint_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Footprint editor (Electronics and EDA). Awaits wire-up.";
    return s;
}

}
