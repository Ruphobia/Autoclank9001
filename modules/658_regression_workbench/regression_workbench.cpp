// SPDX-License-Identifier: GPL-3.0-or-later
#include "regression_workbench.hpp"

namespace regression_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Regression workbench (Data, statistics, and ML). Awaits wire-up.";
    return s;
}

}
