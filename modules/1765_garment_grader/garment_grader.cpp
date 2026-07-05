// SPDX-License-Identifier: GPL-3.0-or-later
#include "garment_grader.hpp"

namespace garment_grader {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Garment Grader (Textiles, sewing, soft crafts). Awaits wire-up.";
    return s;
}

}
