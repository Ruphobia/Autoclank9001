#pragma once

#include <string>

// Stub module for the "Audio format converter" tool entry.
// Signal processing and audio.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace audio_format_converter {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
