#include "adcp_profile_processor.hpp"

namespace adcp_profile_processor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: ADCP profile processor (Oceanography and marine science). Awaits wire-up.";
    return s;
}

}
