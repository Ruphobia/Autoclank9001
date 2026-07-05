// SPDX-License-Identifier: GPL-3.0-or-later
#include "antenna_designer.hpp"

namespace antenna_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Antenna designer (Electronics and EDA). Awaits wire-up.";
    return s;
}

}
