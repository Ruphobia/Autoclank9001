// SPDX-License-Identifier: GPL-3.0-or-later
#include "world_atlas.hpp"

namespace world_atlas {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: World atlas (Writing, publishing, journalism). Awaits wire-up.";
    return s;
}

}
