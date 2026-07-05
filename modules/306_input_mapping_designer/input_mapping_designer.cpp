#include "input_mapping_designer.hpp"

namespace input_mapping_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Input Mapping Designer (Game development). Awaits wire-up.";
    return s;
}

}
