#include "transcription_studio.hpp"

namespace transcription_studio {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Transcription studio (Speech, language, and translation). Awaits wire-up.";
    return s;
}

}
