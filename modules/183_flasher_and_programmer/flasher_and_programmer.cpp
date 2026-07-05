#include "flasher_and_programmer.hpp"

namespace flasher_and_programmer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Flasher and programmer (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
