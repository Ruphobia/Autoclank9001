#include "morphometric_and_aging_tool.hpp"

namespace morphometric_and_aging_tool {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Morphometric and aging tool (Zoology, veterinary, wildlife). Awaits wire-up.";
    return s;
}

}
