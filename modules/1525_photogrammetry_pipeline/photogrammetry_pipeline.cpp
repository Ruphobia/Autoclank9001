#include "photogrammetry_pipeline.hpp"

namespace photogrammetry_pipeline {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Photogrammetry pipeline (Paleontology and archaeology). Awaits wire-up.";
    return s;
}

}
