#include "word_processor.hpp"

namespace word_processor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Word processor (Office productivity). Awaits wire-up.";
    return s;
}

}
