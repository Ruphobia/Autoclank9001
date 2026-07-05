#include "cfd_playground.hpp"

namespace cfd_playground {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: CFD playground (Physics and math). Awaits wire-up.";
    return s;
}

}
