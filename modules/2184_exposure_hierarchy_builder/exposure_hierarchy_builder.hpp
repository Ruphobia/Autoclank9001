#pragma once

#include <string>

// Stub module for the "Exposure hierarchy builder" tool entry.
// Sleep science, wellness, mental health.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace exposure_hierarchy_builder {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
