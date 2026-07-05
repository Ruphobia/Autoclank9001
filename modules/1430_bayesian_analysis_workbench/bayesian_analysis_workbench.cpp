#include "bayesian_analysis_workbench.hpp"

namespace bayesian_analysis_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Bayesian analysis workbench (Psychology and behavioral science). Awaits wire-up.";
    return s;
}

}
