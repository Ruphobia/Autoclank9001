#include "language_and_script_lab.hpp"

namespace language_and_script_lab {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Language And Script Lab (Tabletop RPG and world building). Awaits wire-up.";
    return s;
}

}
