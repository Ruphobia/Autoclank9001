#include "error_propagation_tool.hpp"

namespace error_propagation_tool {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Error propagation tool (Physics and math). Awaits wire-up.";
    return s;
}

}
