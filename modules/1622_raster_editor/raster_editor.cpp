#include "raster_editor.hpp"

namespace raster_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Raster editor (Image and video). Awaits wire-up.";
    return s;
}

}
