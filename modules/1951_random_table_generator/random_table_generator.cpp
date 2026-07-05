#include "random_table_generator.hpp"

namespace random_table_generator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Random Table Generator (Tabletop RPG and world building). Awaits wire-up.";
    return s;
}

}
