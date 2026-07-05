#include "mrz_decoder.hpp"

namespace mrz_decoder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: MRZ Decoder (KYC, identity, compliance verification). Awaits wire-up.";
    return s;
}

}
