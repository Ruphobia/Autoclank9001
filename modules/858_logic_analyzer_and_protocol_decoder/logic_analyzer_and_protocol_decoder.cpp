#include "logic_analyzer_and_protocol_decoder.hpp"

namespace logic_analyzer_and_protocol_decoder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Logic analyzer and protocol decoder (Electronics and EDA). Awaits wire-up.";
    return s;
}

}
