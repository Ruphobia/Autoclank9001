// SPDX-License-Identifier: GPL-3.0-or-later
#include "interferogram_analyzer.hpp"

namespace interferogram_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Interferogram Analyzer (Optics and photonics). Awaits wire-up.";
    return s;
}

}
