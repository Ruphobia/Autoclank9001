#include "multitrack_audio_editor.hpp"

namespace multitrack_audio_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Multitrack audio editor (Signal processing and audio). Awaits wire-up.";
    return s;
}

}
