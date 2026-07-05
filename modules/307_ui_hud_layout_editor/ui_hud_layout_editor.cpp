// SPDX-License-Identifier: GPL-3.0-or-later
#include "ui_hud_layout_editor.hpp"

namespace ui_hud_layout_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: UI/HUD Layout Editor (Game development). Awaits wire-up.";
    return s;
}

}
