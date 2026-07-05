#include "n_body_integrator.hpp"

namespace n_body_integrator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: N-body integrator (Physics and math). Awaits wire-up.";
    return s;
}

}
