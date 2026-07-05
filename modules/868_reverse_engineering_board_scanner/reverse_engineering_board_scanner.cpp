#include "reverse_engineering_board_scanner.hpp"

namespace reverse_engineering_board_scanner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Reverse-engineering board scanner (Electronics and EDA). Awaits wire-up.";
    return s;
}

}
