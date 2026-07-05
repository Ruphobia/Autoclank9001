#pragma once

#include <string>

// Stub module for the "PCB Silkscreen Reader" tool entry.
// Vintage hardware preservation and retrocomputing.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace pcb_silkscreen_reader {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
