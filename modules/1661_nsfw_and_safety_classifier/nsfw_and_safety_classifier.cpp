#include "nsfw_and_safety_classifier.hpp"

namespace nsfw_and_safety_classifier {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: NSFW and safety classifier (Image and video). Awaits wire-up.";
    return s;
}

}
