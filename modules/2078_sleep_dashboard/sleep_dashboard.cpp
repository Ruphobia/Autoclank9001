// SPDX-License-Identifier: GPL-3.0-or-later
#include "sleep_dashboard.hpp"

namespace sleep_dashboard {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sleep dashboard (Sports analytics, fitness, nutrition, coaching). Awaits wire-up.";
    return s;
}

}
