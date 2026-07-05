#include "video_editor.hpp"

namespace video_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Video editor (Image and video). Awaits wire-up.";
    return s;
}

}
