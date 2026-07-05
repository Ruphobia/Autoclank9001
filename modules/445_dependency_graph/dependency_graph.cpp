// SPDX-License-Identifier: GPL-3.0-or-later
#include "dependency_graph.hpp"

namespace dependency_graph {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Dependency graph (Project management). Awaits wire-up.";
    return s;
}

}
