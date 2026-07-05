#pragma once

#include <string>

// Stub module for the "Manuscript variant collator" tool entry.
// Religious studies and comparative religion.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace manuscript_variant_collator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
