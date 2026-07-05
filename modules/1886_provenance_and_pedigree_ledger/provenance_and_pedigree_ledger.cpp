// SPDX-License-Identifier: GPL-3.0-or-later
#include "provenance_and_pedigree_ledger.hpp"

namespace provenance_and_pedigree_ledger {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Provenance and Pedigree Ledger (Numismatics, philately, collecting, antiques). Awaits wire-up.";
    return s;
}

}
