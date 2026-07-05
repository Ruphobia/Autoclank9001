#include "bom_and_sourcing.hpp"

namespace bom_and_sourcing {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: BOM and sourcing (Electronics and EDA). Awaits wire-up.";
    return s;
}

}
