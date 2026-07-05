#include "pronunciation_lexicon.hpp"

namespace pronunciation_lexicon {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Pronunciation lexicon (Speech, language, and translation). Awaits wire-up.";
    return s;
}

}
