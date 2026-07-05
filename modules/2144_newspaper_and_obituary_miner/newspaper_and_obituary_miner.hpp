#pragma once

#include <string>

// Stub module for the "Newspaper and Obituary Miner" tool entry.
// Detailed genealogy and family history.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace newspaper_and_obituary_miner {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
