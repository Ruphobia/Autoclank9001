#include "rom_patch_studio.hpp"

namespace rom_patch_studio {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: ROM Patch Studio (Vintage hardware preservation and retrocomputing). Awaits wire-up.";
    return s;
}

}
