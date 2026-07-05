#include "can_bus_analyzer.hpp"

namespace can_bus_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: CAN Bus Analyzer (Automotive and vehicle systems). Awaits wire-up.";
    return s;
}

}
