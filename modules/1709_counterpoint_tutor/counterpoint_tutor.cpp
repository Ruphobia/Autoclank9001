#include "counterpoint_tutor.hpp"

namespace counterpoint_tutor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Counterpoint Tutor (Music composition and theory). Awaits wire-up.";
    return s;
}

}
