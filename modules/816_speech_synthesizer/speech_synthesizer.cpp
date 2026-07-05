#include "speech_synthesizer.hpp"

namespace speech_synthesizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Speech Synthesizer (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
