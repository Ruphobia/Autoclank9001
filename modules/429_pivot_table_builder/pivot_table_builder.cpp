#include "pivot_table_builder.hpp"

namespace pivot_table_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Pivot table builder (Office productivity). Awaits wire-up.";
    return s;
}

}
