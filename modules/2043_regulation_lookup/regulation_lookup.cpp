#include "regulation_lookup.hpp"

namespace regulation_lookup {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Regulation Lookup (Hunting, fishing, trapping, wildlife tracking). Awaits wire-up.";
    return s;
}

}
