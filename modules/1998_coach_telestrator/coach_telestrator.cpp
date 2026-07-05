// SPDX-License-Identifier: GPL-3.0-or-later
#include "coach_telestrator.hpp"

namespace coach_telestrator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Coach Telestrator (Esports, speedrunning, competitive game analytics). Awaits wire-up.";
    return s;
}

}
