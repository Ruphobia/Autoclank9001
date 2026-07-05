#include "facial_action_coder.hpp"

namespace facial_action_coder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Facial action coder (Psychology and behavioral science). Awaits wire-up.";
    return s;
}

}
