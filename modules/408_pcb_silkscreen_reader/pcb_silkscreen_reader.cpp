#include "pcb_silkscreen_reader.hpp"

namespace pcb_silkscreen_reader {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: PCB Silkscreen Reader (Vintage hardware preservation and retrocomputing). Awaits wire-up.";
    return s;
}

}
