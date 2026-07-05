#include "waveform_viewer.hpp"

namespace waveform_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Waveform viewer (Electronics and EDA). Awaits wire-up.";
    return s;
}

}
