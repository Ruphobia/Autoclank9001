#include "code_and_permit_reference_lookup.hpp"

namespace code_and_permit_reference_lookup {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Code and permit reference lookup (Home and DIY (plumbing, residential electrical, HVAC, landscaping)). Awaits wire-up.";
    return s;
}

}
