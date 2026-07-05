// SPDX-License-Identifier: GPL-3.0-or-later
#include "dmaic_notebook.hpp"

namespace dmaic_notebook {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: DMAIC notebook (Quality, safety, risk, and compliance). Awaits wire-up.";
    return s;
}

}
