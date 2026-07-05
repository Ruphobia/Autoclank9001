// SPDX-License-Identifier: GPL-3.0-or-later
#include "morphology_glosser.hpp"

namespace morphology_glosser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Morphology Glosser (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
