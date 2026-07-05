// SPDX-License-Identifier: GPL-3.0-or-later
#include "item_calibrator.hpp"

namespace item_calibrator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Item Calibrator (Education and pedagogy). Awaits wire-up.";
    return s;
}

}
