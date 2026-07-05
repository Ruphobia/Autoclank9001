#include "periodic_table_explorer.hpp"

namespace periodic_table_explorer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Periodic table explorer (Chemistry and lab). Awaits wire-up.";
    return s;
}

}
