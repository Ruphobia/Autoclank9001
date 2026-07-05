#include "exposure_hierarchy_builder.hpp"

namespace exposure_hierarchy_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Exposure hierarchy builder (Sleep science, wellness, mental health). Awaits wire-up.";
    return s;
}

}
