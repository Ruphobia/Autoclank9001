#include "gif_and_sprite_toolkit.hpp"

namespace gif_and_sprite_toolkit {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: GIF and sprite toolkit (Image and video). Awaits wire-up.";
    return s;
}

}
