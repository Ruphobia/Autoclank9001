// SPDX-License-Identifier: GPL-3.0-or-later
#include "calcium_imaging_analyzer.hpp"

namespace calcium_imaging_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Calcium imaging analyzer (Genomics, synthetic biology, and neuroscience). Awaits wire-up.";
    return s;
}

}
