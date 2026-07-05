// SPDX-License-Identifier: GPL-3.0-or-later
#include "genome_browser.hpp"

namespace genome_browser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Genome Browser (Biology and bioinformatics). Awaits wire-up.";
    return s;
}

}
