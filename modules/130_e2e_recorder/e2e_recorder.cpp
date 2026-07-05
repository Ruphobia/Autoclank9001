#include "e2e_recorder.hpp"

namespace e2e_recorder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: E2E recorder (Web development). Awaits wire-up.";
    return s;
}

}
