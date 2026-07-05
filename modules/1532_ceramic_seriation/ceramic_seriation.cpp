#include "ceramic_seriation.hpp"

namespace ceramic_seriation {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Ceramic seriation (Paleontology and archaeology). Awaits wire-up.";
    return s;
}

}
