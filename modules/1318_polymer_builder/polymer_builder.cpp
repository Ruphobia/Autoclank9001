#include "polymer_builder.hpp"

namespace polymer_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Polymer builder (Chemistry and lab). Awaits wire-up.";
    return s;
}

}
