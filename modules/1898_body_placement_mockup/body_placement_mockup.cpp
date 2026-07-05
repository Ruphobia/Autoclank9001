#include "body_placement_mockup.hpp"

namespace body_placement_mockup {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Body Placement Mockup (Body art: tattoo and body modification). Awaits wire-up.";
    return s;
}

}
