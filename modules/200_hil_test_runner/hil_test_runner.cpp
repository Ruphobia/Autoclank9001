#include "hil_test_runner.hpp"

namespace hil_test_runner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: HIL test runner (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
