// SPDX-License-Identifier: GPL-3.0-or-later
#include "type_generator.hpp"

namespace type_generator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Type generator (Web development). Awaits wire-up.";
    return s;
}

}
