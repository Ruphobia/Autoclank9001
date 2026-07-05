#pragma once

#include <string>

// Stub module for the "TCG deck builder" tool entry.
// Game playing assistants.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace tcg_deck_builder {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
