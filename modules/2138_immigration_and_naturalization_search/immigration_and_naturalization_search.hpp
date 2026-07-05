#pragma once

#include <string>

// Stub module for the "Immigration and Naturalization Search" tool entry.
// Detailed genealogy and family history.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace immigration_and_naturalization_search {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
