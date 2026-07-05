#include "voice_and_ambience_mixer.hpp"

namespace voice_and_ambience_mixer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Voice And Ambience Mixer (Tabletop RPG and world building). Awaits wire-up.";
    return s;
}

}
