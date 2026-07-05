#include "linear_algebra_studio.hpp"

namespace linear_algebra_studio {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Linear algebra studio (Physics and math). Awaits wire-up.";
    return s;
}

}
