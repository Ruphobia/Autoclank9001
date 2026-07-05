// SPDX-License-Identifier: GPL-3.0-or-later
#include "geometric_morphometrics.hpp"

namespace geometric_morphometrics {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Geometric morphometrics (Paleontology and archaeology). Awaits wire-up.";
    return s;
}

}
