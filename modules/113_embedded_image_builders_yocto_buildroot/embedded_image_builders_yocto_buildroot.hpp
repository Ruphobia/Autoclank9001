#pragma once

#include <string>

// Stub module for the "Embedded image builders (Yocto, Buildroot)" tool entry.
// Per-language toolchains.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace embedded_image_builders_yocto_buildroot {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
