#include "codicology_inspector.hpp"

namespace codicology_inspector {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Codicology Inspector (Ancient texts and manuscript studies). Awaits wire-up.";
    return s;
}

}
