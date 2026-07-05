#pragma once

#include <string>

// Stub module for the "Primary Source Reader" tool entry.
// Mythology, folklore, and comparative narrative.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace primary_source_reader {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
