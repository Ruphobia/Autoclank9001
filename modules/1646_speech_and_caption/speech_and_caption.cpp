#include "speech_and_caption.hpp"

namespace speech_and_caption {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Speech and caption (Image and video). Awaits wire-up.";
    return s;
}

}
