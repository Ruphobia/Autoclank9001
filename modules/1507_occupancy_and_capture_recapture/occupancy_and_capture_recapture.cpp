// SPDX-License-Identifier: GPL-3.0-or-later
#include "occupancy_and_capture_recapture.hpp"

namespace occupancy_and_capture_recapture {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Occupancy and capture-recapture (Zoology, veterinary, wildlife). Awaits wire-up.";
    return s;
}

}
