#include "loss_triangle_reserve_estimator.hpp"

namespace loss_triangle_reserve_estimator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Loss triangle reserve estimator (Real estate, insurance, actuarial). Awaits wire-up.";
    return s;
}

}
