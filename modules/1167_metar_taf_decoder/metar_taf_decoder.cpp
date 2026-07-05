#include "metar_taf_decoder.hpp"

namespace metar_taf_decoder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: METAR/TAF decoder (Aviation, amateur radio, and RF). Awaits wire-up.";
    return s;
}

}
