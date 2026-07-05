#include "kanban_board.hpp"

namespace kanban_board {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Kanban board (Office productivity). Awaits wire-up.";
    return s;
}

}
