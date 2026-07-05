#include "chart_builder.hpp"

namespace chart_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Chart builder (Office productivity). Awaits wire-up.";
    return s;
}

}
