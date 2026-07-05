#include "build_runner.hpp"

namespace build_runner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Build runner (Software development). Awaits wire-up.";
    return s;
}

}
