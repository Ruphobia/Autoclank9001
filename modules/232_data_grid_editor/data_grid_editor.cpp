// SPDX-License-Identifier: GPL-3.0-or-later
#include "data_grid_editor.hpp"

namespace data_grid_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Data grid editor (Databases). Awaits wire-up.";
    return s;
}

}
