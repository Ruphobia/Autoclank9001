#pragma once

#include <string>

// Stub module for the "Polyglot Lexicon" tool entry.
// Ancient languages and paleography.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace polyglot_lexicon {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
