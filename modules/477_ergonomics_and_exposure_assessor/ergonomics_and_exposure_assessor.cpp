// SPDX-License-Identifier: GPL-3.0-or-later
#include "ergonomics_and_exposure_assessor.hpp"

namespace ergonomics_and_exposure_assessor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Ergonomics and exposure assessor (Quality, safety, risk, and compliance). Awaits wire-up.";
    return s;
}

}
