#include "curve_fitter.hpp"

namespace curve_fitter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Curve fitter (Physics and math). Awaits wire-up.";
    return s;
}

}
