// SPDX-License-Identifier: GPL-3.0-or-later
#include "concept_map_builder.hpp"

namespace concept_map_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Concept Map Builder (Education and pedagogy). Awaits wire-up.";
    return s;
}

}
