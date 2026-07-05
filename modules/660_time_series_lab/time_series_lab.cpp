// SPDX-License-Identifier: GPL-3.0-or-later
#include "time_series_lab.hpp"

namespace time_series_lab {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Time-series lab (Data, statistics, and ML). Awaits wire-up.";
    return s;
}

}
