#include "trainer.hpp"

namespace trainer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Trainer (Image and video). Awaits wire-up.";
    return s;
}

}
