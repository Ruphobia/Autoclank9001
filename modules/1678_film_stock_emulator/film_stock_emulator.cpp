#include "film_stock_emulator.hpp"

namespace film_stock_emulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Film Stock Emulator (Photography and cinematography). Awaits wire-up.";
    return s;
}

}
