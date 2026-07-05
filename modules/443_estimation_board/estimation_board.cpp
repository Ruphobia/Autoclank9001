#include "estimation_board.hpp"

namespace estimation_board {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Estimation board (Project management). Awaits wire-up.";
    return s;
}

}
