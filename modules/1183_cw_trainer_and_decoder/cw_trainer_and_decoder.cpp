#include "cw_trainer_and_decoder.hpp"

namespace cw_trainer_and_decoder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: CW trainer and decoder (Aviation, amateur radio, and RF). Awaits wire-up.";
    return s;
}

}
