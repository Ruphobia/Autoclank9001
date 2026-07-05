#include "sleight_video_annotator.hpp"

namespace sleight_video_annotator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sleight Video Annotator (Magic, juggling, stage illusion, performance). Awaits wire-up.";
    return s;
}

}
