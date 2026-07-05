#pragma once

#include <string>

// Stub module for the "Music notation editor" tool entry.
// Signal processing and audio.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace music_notation_editor {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
