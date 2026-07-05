// SPDX-License-Identifier: GPL-3.0-or-later
#include "motion_graphics.hpp"

namespace motion_graphics {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Motion graphics (Image and video). Awaits wire-up.";
    return s;
}

}
