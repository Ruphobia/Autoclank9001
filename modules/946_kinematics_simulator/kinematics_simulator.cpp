// SPDX-License-Identifier: GPL-3.0-or-later
#include "kinematics_simulator.hpp"

namespace kinematics_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Kinematics simulator (Mechanical and manufacturing). Awaits wire-up.";
    return s;
}

}
