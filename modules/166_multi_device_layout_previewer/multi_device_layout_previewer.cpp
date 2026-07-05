// SPDX-License-Identifier: GPL-3.0-or-later
#include "multi_device_layout_previewer.hpp"

namespace multi_device_layout_previewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Multi-device layout previewer (Mobile development). Awaits wire-up.";
    return s;
}

}
