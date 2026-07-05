#include "lichess_chess_com_explorer.hpp"

namespace lichess_chess_com_explorer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Lichess/chess.com explorer (Game playing assistants). Awaits wire-up.";
    return s;
}

}
