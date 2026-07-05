#include "bitmap_font_forge.hpp"

namespace bitmap_font_forge {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Bitmap Font Forge (Game development). Awaits wire-up.";
    return s;
}

}
