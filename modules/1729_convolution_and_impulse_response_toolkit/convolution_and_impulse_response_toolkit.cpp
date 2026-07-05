#include "convolution_and_impulse_response_toolkit.hpp"

namespace convolution_and_impulse_response_toolkit {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Convolution and impulse-response toolkit (Signal processing and audio). Awaits wire-up.";
    return s;
}

}
