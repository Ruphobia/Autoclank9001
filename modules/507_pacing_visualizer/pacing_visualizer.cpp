// SPDX-License-Identifier: GPL-3.0-or-later
#include "pacing_visualizer.hpp"

namespace pacing_visualizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Pacing visualizer (Writing, publishing, journalism). Awaits wire-up.";
    return s;
}

}
