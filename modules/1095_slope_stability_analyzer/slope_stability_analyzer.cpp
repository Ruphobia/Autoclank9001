// SPDX-License-Identifier: GPL-3.0-or-later
#include "slope_stability_analyzer.hpp"

namespace slope_stability_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Slope stability analyzer (Heavy engineering: naval, petroleum, mining, nuclear). Awaits wire-up.";
    return s;
}

}
