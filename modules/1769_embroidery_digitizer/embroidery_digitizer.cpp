#include "embroidery_digitizer.hpp"

namespace embroidery_digitizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Embroidery Digitizer (Textiles, sewing, soft crafts). Awaits wire-up.";
    return s;
}

}
