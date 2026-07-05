// SPDX-License-Identifier: GPL-3.0-or-later
#include "pgn_repertoire_workbench.hpp"

namespace pgn_repertoire_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: PGN repertoire workbench (Game playing assistants). Awaits wire-up.";
    return s;
}

}
