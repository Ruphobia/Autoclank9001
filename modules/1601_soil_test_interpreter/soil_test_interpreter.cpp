#include "soil_test_interpreter.hpp"

namespace soil_test_interpreter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Soil Test Interpreter (Agriculture and horticulture). Awaits wire-up.";
    return s;
}

}
