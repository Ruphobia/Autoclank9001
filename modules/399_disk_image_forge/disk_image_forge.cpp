#include "disk_image_forge.hpp"

namespace disk_image_forge {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Disk Image Forge (Vintage hardware preservation and retrocomputing). Awaits wire-up.";
    return s;
}

}
