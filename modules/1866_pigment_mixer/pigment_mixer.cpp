#include "pigment_mixer.hpp"

namespace pigment_mixer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Pigment mixer (Visual arts: calligraphy, painting, sculpture). Awaits wire-up.";
    return s;
}

}
