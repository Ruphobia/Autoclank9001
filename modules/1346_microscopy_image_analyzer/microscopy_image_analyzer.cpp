#include "microscopy_image_analyzer.hpp"

namespace microscopy_image_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Microscopy Image Analyzer (Biology and bioinformatics). Awaits wire-up.";
    return s;
}

}
