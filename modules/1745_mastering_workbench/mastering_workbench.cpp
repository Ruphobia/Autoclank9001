#include "mastering_workbench.hpp"

namespace mastering_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Mastering workbench (Signal processing and audio). Awaits wire-up.";
    return s;
}

}
