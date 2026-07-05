#include "radio_astronomy_toolkit.hpp"

namespace radio_astronomy_toolkit {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Radio Astronomy Toolkit (Astronomy and astrophysics). Awaits wire-up.";
    return s;
}

}
