// SPDX-License-Identifier: GPL-3.0-or-later
#include "load_tester.hpp"

namespace load_tester {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Load tester (Web development). Awaits wire-up.";
    return s;
}

}
