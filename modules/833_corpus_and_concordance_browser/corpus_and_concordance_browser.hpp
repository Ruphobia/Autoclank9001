#pragma once

#include <string>

// Stub module for the "Corpus and concordance browser" tool entry.
// Speech, language, and translation.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace corpus_and_concordance_browser {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
