#include "automl_pipeline.hpp"

namespace automl_pipeline {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: AutoML pipeline (Data, statistics, and ML). Awaits wire-up.";
    return s;
}

}
