#include "resource_classification_tool.hpp"

namespace resource_classification_tool {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Resource classification tool (Heavy engineering: naval, petroleum, mining, nuclear). Awaits wire-up.";
    return s;
}

}
