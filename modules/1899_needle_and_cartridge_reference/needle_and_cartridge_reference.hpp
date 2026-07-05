#pragma once

#include <string>

// Stub module for the "Needle and Cartridge Reference" tool entry.
// Body art: tattoo and body modification.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace needle_and_cartridge_reference {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
