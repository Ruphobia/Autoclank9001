#pragma once

#include <string>

// Stub module for the "SGF library indexer" tool entry.
// Game playing assistants.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace sgf_library_indexer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
