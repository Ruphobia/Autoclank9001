#include "spike_sorter.hpp"

namespace spike_sorter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Spike sorter (Genomics, synthetic biology, and neuroscience). Awaits wire-up.";
    return s;
}

}
