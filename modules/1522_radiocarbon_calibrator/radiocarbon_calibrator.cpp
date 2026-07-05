// SPDX-License-Identifier: GPL-3.0-or-later
#include "radiocarbon_calibrator.hpp"

namespace radiocarbon_calibrator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Radiocarbon calibrator (Paleontology and archaeology). Awaits wire-up.";
    return s;
}

}
