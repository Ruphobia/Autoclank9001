#include "address_verifier.hpp"

namespace address_verifier {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Address Verifier (KYC, identity, compliance verification). Awaits wire-up.";
    return s;
}

}
