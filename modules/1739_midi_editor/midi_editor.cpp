#include "midi_editor.hpp"

namespace midi_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: MIDI editor (Signal processing and audio). Awaits wire-up.";
    return s;
}

}
