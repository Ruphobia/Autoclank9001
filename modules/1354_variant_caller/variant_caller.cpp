// SPDX-License-Identifier: GPL-3.0-or-later
#include "variant_caller.hpp"

namespace variant_caller {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Variant caller (Microbiology, virology, and immunology). Awaits wire-up.";
    return s;
}

}
