#include "dubbing_studio.hpp"

namespace dubbing_studio {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Dubbing studio (Speech, language, and translation). Awaits wire-up.";
    return s;
}

}
