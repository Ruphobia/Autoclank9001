#include "translation_memory.hpp"

namespace translation_memory {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Translation memory (Speech, language, and translation). Awaits wire-up.";
    return s;
}

}
