#include "handout_and_prop_studio.hpp"

namespace handout_and_prop_studio {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Handout And Prop Studio (Tabletop RPG and world building). Awaits wire-up.";
    return s;
}

}
