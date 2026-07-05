#include "bmp_sizer.hpp"

namespace bmp_sizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: BMP sizer (Civil utility (water, waste, sanitation)). Awaits wire-up.";
    return s;
}

}
