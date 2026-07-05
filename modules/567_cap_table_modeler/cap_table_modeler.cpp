#include "cap_table_modeler.hpp"

namespace cap_table_modeler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Cap Table Modeler (Finance, markets, accounting). Awaits wire-up.";
    return s;
}

}
