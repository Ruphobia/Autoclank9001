#include "randomizer_seed_analyzer.hpp"

namespace randomizer_seed_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Randomizer seed analyzer (Game playing assistants). Awaits wire-up.";
    return s;
}

}
