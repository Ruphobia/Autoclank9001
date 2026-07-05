#include "segmenter.hpp"

namespace segmenter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Segmenter (Image and video). Awaits wire-up.";
    return s;
}

}
