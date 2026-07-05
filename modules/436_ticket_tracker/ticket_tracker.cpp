// SPDX-License-Identifier: GPL-3.0-or-later
#include "ticket_tracker.hpp"

namespace ticket_tracker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Ticket tracker (Project management). Awaits wire-up.";
    return s;
}

}
