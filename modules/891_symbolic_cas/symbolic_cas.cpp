#include "symbolic_cas.hpp"

namespace symbolic_cas {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Symbolic CAS (Physics and math). Awaits wire-up.";
    return s;
}

}
