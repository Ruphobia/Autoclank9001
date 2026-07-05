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
