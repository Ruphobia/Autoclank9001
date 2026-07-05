#include "diarization_and_speaker_tool.hpp"

namespace diarization_and_speaker_tool {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Diarization and speaker tool (Speech, language, and translation). Awaits wire-up.";
    return s;
}

}
