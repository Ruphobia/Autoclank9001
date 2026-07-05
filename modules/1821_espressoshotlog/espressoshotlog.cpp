// SPDX-License-Identifier: GPL-3.0-or-later
#include "espressoshotlog.hpp"

namespace espressoshotlog {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: EspressoShotLog (Coffee, tea, wine, spirits, beverages). Awaits wire-up.";
    return s;
}

}
