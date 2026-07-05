#include "gait_and_form_video_analyzer.hpp"

namespace gait_and_form_video_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Gait and form video analyzer (Sports analytics, fitness, nutrition, coaching). Awaits wire-up.";
    return s;
}

}
