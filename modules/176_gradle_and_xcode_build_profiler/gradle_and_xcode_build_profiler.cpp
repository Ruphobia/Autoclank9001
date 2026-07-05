#include "gradle_and_xcode_build_profiler.hpp"

namespace gradle_and_xcode_build_profiler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Gradle and Xcode build profiler (Mobile development). Awaits wire-up.";
    return s;
}

}
