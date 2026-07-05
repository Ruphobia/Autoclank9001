#include "disk_image_browser.hpp"

namespace disk_image_browser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Disk Image Browser (Cybersecurity and digital forensics). Awaits wire-up.";
    return s;
}

}
