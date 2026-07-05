#include "pdf417_barcode_reader.hpp"

namespace pdf417_barcode_reader {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: PDF417 Barcode Reader (KYC, identity, compliance verification). Awaits wire-up.";
    return s;
}

}
