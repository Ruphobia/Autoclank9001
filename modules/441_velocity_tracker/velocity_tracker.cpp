// SPDX-License-Identifier: GPL-3.0-or-later
#include "velocity_tracker.hpp"

namespace velocity_tracker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Velocity tracker (Project management). Awaits wire-up.";
    return s;
}

}
