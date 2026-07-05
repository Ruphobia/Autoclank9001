#include "fuzzer_and_property_tester.hpp"

namespace fuzzer_and_property_tester {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Fuzzer and property tester (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
