#include "drawing_generator.hpp"

namespace drawing_generator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Drawing generator (Mechanical and manufacturing). Awaits wire-up.";
    return s;
}

}
