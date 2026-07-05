// SPDX-License-Identifier: GPL-3.0-or-later
#include "mrp_estimator.hpp"

namespace mrp_estimator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: MRP estimator (Sociology, political science, public policy). Awaits wire-up.";
    return s;
}

}
