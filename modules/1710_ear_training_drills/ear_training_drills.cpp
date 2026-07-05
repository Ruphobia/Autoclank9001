#include "ear_training_drills.hpp"

namespace ear_training_drills {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Ear Training Drills (Music composition and theory). Awaits wire-up.";
    return s;
}

}
