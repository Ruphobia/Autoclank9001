#include "ais_decoder_and_viewer.hpp"

namespace ais_decoder_and_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: AIS decoder and viewer (Sailing, boating, navigation). Awaits wire-up.";
    return s;
}

}
