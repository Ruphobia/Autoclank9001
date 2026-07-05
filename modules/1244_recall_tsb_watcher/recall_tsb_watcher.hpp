#pragma once

#include <string>

// Stub module for the "Recall & TSB Watcher" tool entry.
// Automotive and vehicle systems.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace recall_tsb_watcher {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
