#include "epassport_chip_reader.hpp"

namespace epassport_chip_reader {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: ePassport Chip Reader (KYC, identity, compliance verification). Awaits wire-up.";
    return s;
}

}
