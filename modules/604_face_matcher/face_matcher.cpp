#include "face_matcher.hpp"

namespace face_matcher {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Face Matcher (KYC, identity, compliance verification). Awaits wire-up.";
    return s;
}

}
