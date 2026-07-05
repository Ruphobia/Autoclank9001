#include "ideal_point_estimator.hpp"

namespace ideal_point_estimator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Ideal-point estimator (Sociology, political science, public policy). Awaits wire-up.";
    return s;
}

}
