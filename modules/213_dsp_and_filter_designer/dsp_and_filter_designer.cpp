#include "dsp_and_filter_designer.hpp"

namespace dsp_and_filter_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: DSP and filter designer (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
