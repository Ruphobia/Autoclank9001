#include "input_lab.hpp"

namespace input_lab {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Input Lab (Esports, speedrunning, competitive game analytics). Awaits wire-up.";
    return s;
}

}
