#pragma once

#include <string>

// Stub module for the "Morphology and lemma analyzer" tool entry.
// Speech, language, and translation.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace morphology_and_lemma_analyzer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
