#include "multispectral_image_stack.hpp"

namespace multispectral_image_stack {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Multispectral Image Stack (Ancient texts and manuscript studies). Awaits wire-up.";
    return s;
}

}
