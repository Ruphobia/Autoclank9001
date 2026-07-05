#pragma once

#include <string>

// Stub module for the "Bitmap Font Forge" tool entry.
// Game development.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace bitmap_font_forge {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
