#include "speech_toolkit.hpp"

namespace speech_toolkit {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Speech toolkit (Signal processing and audio). Awaits wire-up.";
    return s;
}

}
