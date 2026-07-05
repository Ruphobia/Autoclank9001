#include "practice_logger.hpp"

namespace practice_logger {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Practice Logger (Esports, speedrunning, competitive game analytics). Awaits wire-up.";
    return s;
}

}
