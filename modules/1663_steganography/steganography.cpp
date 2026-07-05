#include "steganography.hpp"

namespace steganography {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Steganography (Image and video). Awaits wire-up.";
    return s;
}

}
