#include "embedded_image_builders_yocto_buildroot.hpp"

namespace embedded_image_builders_yocto_buildroot {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Embedded image builders (Yocto, Buildroot) (Per-language toolchains). Awaits wire-up.";
    return s;
}

}
