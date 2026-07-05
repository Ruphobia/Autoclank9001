#include "prompt_lab.hpp"

namespace prompt_lab {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Prompt lab (Data, statistics, and ML). Awaits wire-up.";
    return s;
}

}
