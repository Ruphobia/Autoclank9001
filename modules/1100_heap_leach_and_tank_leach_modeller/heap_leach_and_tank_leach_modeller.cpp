#include "heap_leach_and_tank_leach_modeller.hpp"

namespace heap_leach_and_tank_leach_modeller {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Heap-leach and tank-leach modeller (Heavy engineering: naval, petroleum, mining, nuclear). Awaits wire-up.";
    return s;
}

}
