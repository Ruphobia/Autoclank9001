#include "metabolic_model_workbench.hpp"

namespace metabolic_model_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Metabolic model workbench (Microbiology, virology, and immunology). Awaits wire-up.";
    return s;
}

}
