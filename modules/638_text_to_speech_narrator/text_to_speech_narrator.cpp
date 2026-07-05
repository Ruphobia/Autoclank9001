#include "text_to_speech_narrator.hpp"

namespace text_to_speech_narrator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Text-to-speech narrator (Documents and reading). Awaits wire-up.";
    return s;
}

}
