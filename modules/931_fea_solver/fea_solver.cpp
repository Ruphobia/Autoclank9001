// SPDX-License-Identifier: GPL-3.0-or-later
#include "fea_solver.hpp"

namespace fea_solver {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: FEA solver (Mechanical and manufacturing). Awaits wire-up.";
    return s;
}

}
