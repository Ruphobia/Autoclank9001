#include "tournament_bracket.hpp"

namespace tournament_bracket {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Tournament Bracket (Esports, speedrunning, competitive game analytics). Awaits wire-up.";
    return s;
}

}
