#include "dsp_code_playground.hpp"

namespace dsp_code_playground {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: DSP code playground (Signal processing and audio). Awaits wire-up.";
    return s;
}

}
