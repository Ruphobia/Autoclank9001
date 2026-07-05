#include "slide_composer.hpp"

namespace slide_composer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Slide composer (Office productivity). Awaits wire-up.";
    return s;
}

}
