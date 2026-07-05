// SPDX-License-Identifier: GPL-3.0-or-later
#include "gradient_designer.hpp"

namespace gradient_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Gradient designer (Visual arts: calligraphy, painting, sculpture). Awaits wire-up.";
    return s;
}

}
