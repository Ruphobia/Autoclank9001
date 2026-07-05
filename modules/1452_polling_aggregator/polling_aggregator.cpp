// SPDX-License-Identifier: GPL-3.0-or-later
#include "polling_aggregator.hpp"

namespace polling_aggregator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Polling aggregator (Sociology, political science, public policy). Awaits wire-up.";
    return s;
}

}
