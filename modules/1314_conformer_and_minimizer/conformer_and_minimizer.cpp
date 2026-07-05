#include "conformer_and_minimizer.hpp"

namespace conformer_and_minimizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Conformer and minimizer (Chemistry and lab). Awaits wire-up.";
    return s;
}

}
