#include "sail_trim_assistant.hpp"

namespace sail_trim_assistant {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sail trim assistant (Sailing, boating, navigation). Awaits wire-up.";
    return s;
}

}
