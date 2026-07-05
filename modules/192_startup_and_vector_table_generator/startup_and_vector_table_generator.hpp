#pragma once

#include <string>

// Stub module for the "Startup and vector table generator" tool entry.
// Firmware and embedded.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace startup_and_vector_table_generator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
