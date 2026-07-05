#include "emulator_harness.hpp"

namespace emulator_harness {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Emulator Harness (Vintage hardware preservation and retrocomputing). Awaits wire-up.";
    return s;
}

}
