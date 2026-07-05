#include "rules_oracle.hpp"

namespace rules_oracle {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Rules Oracle (Tabletop RPG and world building). Awaits wire-up.";
    return s;
}

}
