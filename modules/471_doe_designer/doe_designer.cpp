// SPDX-License-Identifier: GPL-3.0-or-later
#include "doe_designer.hpp"

namespace doe_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: DOE designer (Quality, safety, risk, and compliance). Awaits wire-up.";
    return s;
}

}
