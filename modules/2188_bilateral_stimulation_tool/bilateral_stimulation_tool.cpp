#include "bilateral_stimulation_tool.hpp"

namespace bilateral_stimulation_tool {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Bilateral stimulation tool (Sleep science, wellness, mental health). Awaits wire-up.";
    return s;
}

}
