#include "tts_bench.hpp"

namespace tts_bench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: TTS bench (Speech, language, and translation). Awaits wire-up.";
    return s;
}

}
