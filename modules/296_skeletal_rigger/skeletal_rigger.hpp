#pragma once

#include <string>

// Stub module for the "Skeletal Rigger" tool entry.
// Game development.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace skeletal_rigger {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
