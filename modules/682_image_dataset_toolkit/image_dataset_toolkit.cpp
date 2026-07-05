#include "image_dataset_toolkit.hpp"

namespace image_dataset_toolkit {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Image dataset toolkit (Data, statistics, and ML). Awaits wire-up.";
    return s;
}

}
