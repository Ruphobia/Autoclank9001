#pragma once

#include <string>

// Stub module for the "Multi-device layout previewer" tool entry.
// Mobile development.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace multi_device_layout_previewer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
