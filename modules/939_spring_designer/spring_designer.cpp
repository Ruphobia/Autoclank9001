#include "spring_designer.hpp"

namespace spring_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Spring designer (Mechanical and manufacturing). Awaits wire-up.";
    return s;
}

}
