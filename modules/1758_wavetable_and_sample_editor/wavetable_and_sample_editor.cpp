#include "wavetable_and_sample_editor.hpp"

namespace wavetable_and_sample_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Wavetable and sample editor (Signal processing and audio). Awaits wire-up.";
    return s;
}

}
