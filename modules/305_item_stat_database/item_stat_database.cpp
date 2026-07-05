// SPDX-License-Identifier: GPL-3.0-or-later
#include "item_stat_database.hpp"

namespace item_stat_database {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Item & Stat Database (Game development). Awaits wire-up.";
    return s;
}

}
