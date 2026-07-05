#include "hypnogram_viewer.hpp"

namespace hypnogram_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Hypnogram Viewer (Sleep science, wellness, mental health). Awaits wire-up.";
    return s;
}

}
