// SPDX-License-Identifier: GPL-3.0-or-later
#include "geochronology_calculator.hpp"

namespace geochronology_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Geochronology calculator (Paleontology and archaeology). Awaits wire-up.";
    return s;
}

}
