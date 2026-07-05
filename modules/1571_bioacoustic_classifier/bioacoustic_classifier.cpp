#include "bioacoustic_classifier.hpp"

namespace bioacoustic_classifier {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Bioacoustic classifier (Ecology, conservation, sustainability). Awaits wire-up.";
    return s;
}

}
