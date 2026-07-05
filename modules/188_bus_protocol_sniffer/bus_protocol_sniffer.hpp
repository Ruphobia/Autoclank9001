#pragma once

#include <string>

// Stub module for the "Bus protocol sniffer" tool entry.
// Firmware and embedded.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace bus_protocol_sniffer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
