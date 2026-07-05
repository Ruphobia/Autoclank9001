#include "crystallography_viewer.hpp"

namespace crystallography_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Crystallography viewer (Chemistry and lab). Awaits wire-up.";
    return s;
}

}
