#include "startup_and_vector_table_generator.hpp"

namespace startup_and_vector_table_generator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Startup and vector table generator (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
