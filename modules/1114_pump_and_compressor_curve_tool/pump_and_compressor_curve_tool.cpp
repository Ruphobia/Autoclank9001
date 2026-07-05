#include "pump_and_compressor_curve_tool.hpp"

namespace pump_and_compressor_curve_tool {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Pump and compressor curve tool (Heavy engineering: naval, petroleum, mining, nuclear). Awaits wire-up.";
    return s;
}

}
