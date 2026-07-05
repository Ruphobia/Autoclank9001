// SPDX-License-Identifier: GPL-3.0-or-later
#include "static_analyzer.hpp"

namespace static_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Static analyzer (Software development). Awaits wire-up.";
    return s;
}

}
