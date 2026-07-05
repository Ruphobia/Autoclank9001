#include "solunar_tables.hpp"

namespace solunar_tables {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Solunar Tables (Hunting, fishing, trapping, wildlife tracking). Awaits wire-up.";
    return s;
}

}
