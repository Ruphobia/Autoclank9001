#include "collation_engine.hpp"

namespace collation_engine {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Collation Engine (Ancient texts and manuscript studies). Awaits wire-up.";
    return s;
}

}
