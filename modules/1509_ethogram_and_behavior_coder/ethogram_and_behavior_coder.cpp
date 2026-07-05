#include "ethogram_and_behavior_coder.hpp"

namespace ethogram_and_behavior_coder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Ethogram and behavior coder (Zoology, veterinary, wildlife). Awaits wire-up.";
    return s;
}

}
