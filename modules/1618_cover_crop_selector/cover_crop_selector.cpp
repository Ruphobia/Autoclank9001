#include "cover_crop_selector.hpp"

namespace cover_crop_selector {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Cover Crop Selector (Agriculture and horticulture). Awaits wire-up.";
    return s;
}

}
