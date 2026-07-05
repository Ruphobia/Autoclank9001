// SPDX-License-Identifier: GPL-3.0-or-later
#include "taxidermy_notebook.hpp"

namespace taxidermy_notebook {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Taxidermy Notebook (Hunting, fishing, trapping, wildlife tracking). Awaits wire-up.";
    return s;
}

}
