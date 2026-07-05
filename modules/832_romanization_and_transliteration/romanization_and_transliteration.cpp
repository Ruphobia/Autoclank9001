#include "romanization_and_transliteration.hpp"

namespace romanization_and_transliteration {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Romanization and transliteration (Speech, language, and translation). Awaits wire-up.";
    return s;
}

}
