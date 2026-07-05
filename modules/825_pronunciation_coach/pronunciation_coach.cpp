#include "pronunciation_coach.hpp"

namespace pronunciation_coach {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Pronunciation coach (Speech, language, and translation). Awaits wire-up.";
    return s;
}

}
