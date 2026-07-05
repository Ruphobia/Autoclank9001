#include "raw_developer.hpp"

namespace raw_developer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: RAW developer (Image and video). Awaits wire-up.";
    return s;
}

}
