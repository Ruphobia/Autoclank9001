#include "experiment_timeline_builder.hpp"

namespace experiment_timeline_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Experiment timeline builder (Psychology and behavioral science). Awaits wire-up.";
    return s;
}

}
