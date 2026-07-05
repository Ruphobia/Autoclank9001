#include "reframing_assistant.hpp"

namespace reframing_assistant {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Reframing assistant (Sleep science, wellness, mental health). Awaits wire-up.";
    return s;
}

}
