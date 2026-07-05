#include "voice_leading_checker.hpp"

namespace voice_leading_checker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Voice Leading Checker (Music composition and theory). Awaits wire-up.";
    return s;
}

}
