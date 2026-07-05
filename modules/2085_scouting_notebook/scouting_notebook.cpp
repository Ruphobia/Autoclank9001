#include "scouting_notebook.hpp"

namespace scouting_notebook {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Scouting notebook (Sports analytics, fitness, nutrition, coaching). Awaits wire-up.";
    return s;
}

}
