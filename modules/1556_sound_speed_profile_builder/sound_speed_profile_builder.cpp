#include "sound_speed_profile_builder.hpp"

namespace sound_speed_profile_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sound speed profile builder (Oceanography and marine science). Awaits wire-up.";
    return s;
}

}
