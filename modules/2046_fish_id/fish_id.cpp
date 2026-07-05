#include "fish_id.hpp"

namespace fish_id {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Fish ID (Hunting, fishing, trapping, wildlife tracking). Awaits wire-up.";
    return s;
}

}
