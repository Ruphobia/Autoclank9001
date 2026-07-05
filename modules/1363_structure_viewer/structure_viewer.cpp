#include "structure_viewer.hpp"

namespace structure_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Structure viewer (Microbiology, virology, and immunology). Awaits wire-up.";
    return s;
}

}
