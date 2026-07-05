#include "draft_analyzer.hpp"

namespace draft_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Draft Analyzer (Esports, speedrunning, competitive game analytics). Awaits wire-up.";
    return s;
}

}
