#include "signal_integrity_analyzer.hpp"

namespace signal_integrity_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Signal integrity analyzer (Electronics and EDA). Awaits wire-up.";
    return s;
}

}
