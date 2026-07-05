#include "caption_renderer.hpp"

namespace caption_renderer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Caption renderer (Speech, language, and translation). Awaits wire-up.";
    return s;
}

}
