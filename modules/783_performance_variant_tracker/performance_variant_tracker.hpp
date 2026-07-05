#pragma once

#include <string>

// Stub module for the "Performance Variant Tracker" tool entry.
// Mythology, folklore, and comparative narrative.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace performance_variant_tracker {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
