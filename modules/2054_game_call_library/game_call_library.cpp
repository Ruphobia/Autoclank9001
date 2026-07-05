#include "game_call_library.hpp"

namespace game_call_library {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Game Call Library (Hunting, fishing, trapping, wildlife tracking). Awaits wire-up.";
    return s;
}

}
