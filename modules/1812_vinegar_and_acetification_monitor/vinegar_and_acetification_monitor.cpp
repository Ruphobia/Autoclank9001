// SPDX-License-Identifier: GPL-3.0-or-later
#include "vinegar_and_acetification_monitor.hpp"

namespace vinegar_and_acetification_monitor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Vinegar and Acetification Monitor (Cooking, brewing, fermentation, food science). Awaits wire-up.";
    return s;
}

}
