// SPDX-License-Identifier: GPL-3.0-or-later
#include "burndown_and_burnup_charts.hpp"

namespace burndown_and_burnup_charts {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Burndown and burnup charts (Project management). Awaits wire-up.";
    return s;
}

}
