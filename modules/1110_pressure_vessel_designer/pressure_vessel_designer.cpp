// SPDX-License-Identifier: GPL-3.0-or-later
#include "pressure_vessel_designer.hpp"

namespace pressure_vessel_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Pressure vessel designer (Heavy engineering: naval, petroleum, mining, nuclear). Awaits wire-up.";
    return s;
}

}
