#include "effects_chain.hpp"

namespace effects_chain {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Effects chain (Signal processing and audio). Awaits wire-up.";
    return s;
}

}
