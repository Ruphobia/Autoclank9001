#include "inequality_and_segregation_metrics.hpp"

namespace inequality_and_segregation_metrics {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Inequality and segregation metrics (Sociology, political science, public policy). Awaits wire-up.";
    return s;
}

}
