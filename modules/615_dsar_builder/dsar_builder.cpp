#include "dsar_builder.hpp"

namespace dsar_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: DSAR Builder (KYC, identity, compliance verification). Awaits wire-up.";
    return s;
}

}
