#include "prompt_library.hpp"

namespace prompt_library {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Prompt library (Image and video). Awaits wire-up.";
    return s;
}

}
