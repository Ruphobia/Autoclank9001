#include "back_translation_qa.hpp"

namespace back_translation_qa {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Back-translation QA (Speech, language, and translation). Awaits wire-up.";
    return s;
}

}
