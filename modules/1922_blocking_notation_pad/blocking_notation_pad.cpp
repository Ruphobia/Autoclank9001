#include "blocking_notation_pad.hpp"

namespace blocking_notation_pad {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Blocking Notation Pad (Performing arts: theater, sound, lighting, stage). Awaits wire-up.";
    return s;
}

}
