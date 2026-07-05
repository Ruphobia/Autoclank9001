// SPDX-License-Identifier: GPL-3.0-or-later
#include "materials_property_database.hpp"

namespace materials_property_database {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Materials property database (Heavy engineering: naval, petroleum, mining, nuclear). Awaits wire-up.";
    return s;
}

}
