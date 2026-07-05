#include "autoguider.hpp"

namespace autoguider {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Autoguider (Astronomy and astrophysics). Awaits wire-up.";
    return s;
}

}
