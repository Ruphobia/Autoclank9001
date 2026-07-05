#include "audio_to_midi_transcription.hpp"

namespace audio_to_midi_transcription {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Audio-to-MIDI transcription (Signal processing and audio). Awaits wire-up.";
    return s;
}

}
