#include "g2p_and_phonetic_transcriber.hpp"

namespace g2p_and_phonetic_transcriber {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: G2P and phonetic transcriber (Speech, language, and translation). Awaits wire-up.";
    return s;
}

}
