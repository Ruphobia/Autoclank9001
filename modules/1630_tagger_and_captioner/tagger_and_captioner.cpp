#include "tagger_and_captioner.hpp"

namespace tagger_and_captioner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Tagger and captioner (Image and video). Awaits wire-up.";
    return s;
}

}
