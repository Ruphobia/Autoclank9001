#pragma once

#include <string>

// Stub module for the "GIF and sprite toolkit" tool entry.
// Image and video.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace gif_and_sprite_toolkit {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
