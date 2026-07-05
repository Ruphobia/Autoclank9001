#include "signal_generator.hpp"

namespace signal_generator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Signal generator (Signal processing and audio). Awaits wire-up.";
    return s;
}

}
