#pragma once

#include <string>

// Stub module for the "Necropsy report builder" tool entry.
// Zoology, veterinary, wildlife.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace necropsy_report_builder {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
