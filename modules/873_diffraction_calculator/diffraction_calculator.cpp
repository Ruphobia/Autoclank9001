// SPDX-License-Identifier: GPL-3.0-or-later
#include "diffraction_calculator.hpp"

namespace diffraction_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Diffraction Calculator (Optics and photonics). Awaits wire-up.";
    return s;
}

}
