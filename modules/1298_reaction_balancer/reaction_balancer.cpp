#include "reaction_balancer.hpp"

namespace reaction_balancer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Reaction balancer (Chemistry and lab). Awaits wire-up.";
    return s;
}

}
