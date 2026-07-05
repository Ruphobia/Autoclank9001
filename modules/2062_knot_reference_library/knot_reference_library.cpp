// SPDX-License-Identifier: GPL-3.0-or-later
#include "knot_reference_library.hpp"

namespace knot_reference_library {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Knot Reference Library (Outdoor recreation: camping, climbing, skiing, paragliding, geocaching). Awaits wire-up.";
    return s;
}

}
