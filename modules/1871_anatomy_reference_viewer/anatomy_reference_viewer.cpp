#include "anatomy_reference_viewer.hpp"

namespace anatomy_reference_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Anatomy reference viewer (Visual arts: calligraphy, painting, sculpture). Awaits wire-up.";
    return s;
}

}
