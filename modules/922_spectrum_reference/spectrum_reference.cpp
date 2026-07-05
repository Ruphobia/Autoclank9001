#include "spectrum_reference.hpp"

namespace spectrum_reference {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Spectrum reference (Physics and math). Awaits wire-up.";
    return s;
}

}
