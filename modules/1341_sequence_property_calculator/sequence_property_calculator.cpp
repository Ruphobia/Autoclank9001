// SPDX-License-Identifier: GPL-3.0-or-later
#include "sequence_property_calculator.hpp"

namespace sequence_property_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sequence Property Calculator (Biology and bioinformatics). Awaits wire-up.";
    return s;
}

}
