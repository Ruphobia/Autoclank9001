#include "parallel_text_viewer.hpp"

namespace parallel_text_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Parallel text viewer (Religious studies and comparative religion). Awaits wire-up.";
    return s;
}

}
