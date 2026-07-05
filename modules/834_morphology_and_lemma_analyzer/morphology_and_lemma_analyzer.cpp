#include "morphology_and_lemma_analyzer.hpp"

namespace morphology_and_lemma_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Morphology and lemma analyzer (Speech, language, and translation). Awaits wire-up.";
    return s;
}

}
