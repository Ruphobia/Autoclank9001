#include "bsp_and_board_generator.hpp"

namespace bsp_and_board_generator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: BSP and board generator (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
