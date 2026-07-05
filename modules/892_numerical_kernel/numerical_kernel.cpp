#include "numerical_kernel.hpp"

namespace numerical_kernel {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Numerical kernel (Physics and math). Awaits wire-up.";
    return s;
}

}
