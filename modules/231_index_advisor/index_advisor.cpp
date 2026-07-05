// SPDX-License-Identifier: GPL-3.0-or-later
#include "index_advisor.hpp"

namespace index_advisor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Index advisor (Databases). Awaits wire-up.";
    return s;
}

}
