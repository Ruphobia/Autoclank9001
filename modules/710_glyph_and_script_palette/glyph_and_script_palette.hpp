#pragma once

#include <string>

// Stub module for the "Glyph and Script Palette" tool entry.
// Ancient languages and paleography.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace glyph_and_script_palette {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
