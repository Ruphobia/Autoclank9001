#pragma once

#include <string>

// Stub module for the "Convolution and impulse-response toolkit" tool entry.
// Signal processing and audio.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace convolution_and_impulse_response_toolkit {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
