// SPDX-License-Identifier: GPL-3.0-or-later
#include "symbol_concordance.hpp"

namespace symbol_concordance {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Symbol Concordance (Mythology, folklore, and comparative narrative). Awaits wire-up.";
    return s;
}

}
