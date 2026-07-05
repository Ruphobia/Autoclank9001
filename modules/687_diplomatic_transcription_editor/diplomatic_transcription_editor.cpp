#include "diplomatic_transcription_editor.hpp"

namespace diplomatic_transcription_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Diplomatic transcription editor (History and archival research). Awaits wire-up.";
    return s;
}

}
