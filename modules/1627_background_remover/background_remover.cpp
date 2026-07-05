#include "background_remover.hpp"

namespace background_remover {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Background remover (Image and video). Awaits wire-up.";
    return s;
}

}
