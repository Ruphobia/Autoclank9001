#include "voice_activity_and_segmenter.hpp"

namespace voice_activity_and_segmenter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Voice activity and segmenter (Speech, language, and translation). Awaits wire-up.";
    return s;
}

}
