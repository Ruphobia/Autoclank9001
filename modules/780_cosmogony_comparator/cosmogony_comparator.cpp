#include "cosmogony_comparator.hpp"

namespace cosmogony_comparator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Cosmogony Comparator (Mythology, folklore, and comparative narrative). Awaits wire-up.";
    return s;
}

}
