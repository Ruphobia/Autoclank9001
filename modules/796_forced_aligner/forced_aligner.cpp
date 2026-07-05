#include "forced_aligner.hpp"

namespace forced_aligner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Forced Aligner (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
