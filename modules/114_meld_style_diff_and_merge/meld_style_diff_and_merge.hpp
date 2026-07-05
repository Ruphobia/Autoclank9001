#pragma once

#include <string>

// Stub module for the "Meld-style diff and merge" tool entry.
// Per-language toolchains.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace meld_style_diff_and_merge {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
