// SPDX-License-Identifier: GPL-3.0-or-later
#include "circuit_equivalence_checker.hpp"

namespace circuit_equivalence_checker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Circuit Equivalence Checker (Quantum computing). Awaits wire-up.";
    return s;
}

}
