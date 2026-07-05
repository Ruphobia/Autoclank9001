// SPDX-License-Identifier: GPL-3.0-or-later
#include "poker_equity_solver.hpp"

namespace poker_equity_solver {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Poker equity solver (Game playing assistants). Awaits wire-up.";
    return s;
}

}
