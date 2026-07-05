#include "primary_source_reader.hpp"

namespace primary_source_reader {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Primary Source Reader (Mythology, folklore, and comparative narrative). Awaits wire-up.";
    return s;
}

}
