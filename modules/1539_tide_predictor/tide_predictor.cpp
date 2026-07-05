// SPDX-License-Identifier: GPL-3.0-or-later
#include "tide_predictor.hpp"

namespace tide_predictor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Tide predictor (Oceanography and marine science). Awaits wire-up.";
    return s;
}

}
