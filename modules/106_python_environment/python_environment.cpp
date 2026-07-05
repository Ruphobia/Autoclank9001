#include "python_environment.hpp"

namespace python_environment {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Python environment (Per-language toolchains). Awaits wire-up.";
    return s;
}

}
