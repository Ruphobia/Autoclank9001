// SPDX-License-Identifier: GPL-3.0-or-later
#include "whiteboard.hpp"

namespace whiteboard {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Whiteboard (Office productivity). Awaits wire-up.";
    return s;
}

}
