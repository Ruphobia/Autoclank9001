#include "soiling_snow_degradation_model.hpp"

namespace soiling_snow_degradation_model {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Soiling, Snow & Degradation Model (Energy systems). Awaits wire-up.";
    return s;
}

}
