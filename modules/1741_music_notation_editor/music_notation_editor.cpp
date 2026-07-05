#include "music_notation_editor.hpp"

namespace music_notation_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Music notation editor (Signal processing and audio). Awaits wire-up.";
    return s;
}

}
