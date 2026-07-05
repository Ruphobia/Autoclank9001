#include "test_runner.hpp"

namespace test_runner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Test runner (Software development). Awaits wire-up.";
    return s;
}

}
