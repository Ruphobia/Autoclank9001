#include "welding_procedure_helper.hpp"

namespace welding_procedure_helper {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Welding Procedure Helper (Materials science and metallurgy). Awaits wire-up.";
    return s;
}

}
