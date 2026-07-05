#include "sprite_atlas_packer.hpp"

namespace sprite_atlas_packer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sprite Atlas Packer (Game development). Awaits wire-up.";
    return s;
}

}
