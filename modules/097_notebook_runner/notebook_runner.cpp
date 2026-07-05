#include "notebook_runner.hpp"

namespace notebook_runner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Notebook runner (Software development). Awaits wire-up.";
    return s;
}

}
