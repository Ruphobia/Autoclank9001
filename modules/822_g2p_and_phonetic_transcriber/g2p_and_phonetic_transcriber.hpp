#pragma once

#include <string>

// Stub module for the "G2P and phonetic transcriber" tool entry.
// Speech, language, and translation.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace g2p_and_phonetic_transcriber {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
