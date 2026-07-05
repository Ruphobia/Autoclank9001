#pragma once

#include <string>

// Stub module for the "Disk Image Browser" tool entry.
// Cybersecurity and digital forensics.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace disk_image_browser {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
