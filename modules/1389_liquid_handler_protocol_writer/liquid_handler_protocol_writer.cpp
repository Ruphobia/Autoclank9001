#include "liquid_handler_protocol_writer.hpp"

namespace liquid_handler_protocol_writer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Liquid-handler protocol writer (Genomics, synthetic biology, and neuroscience). Awaits wire-up.";
    return s;
}

}
