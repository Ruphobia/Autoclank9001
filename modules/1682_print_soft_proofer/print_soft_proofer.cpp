#include "print_soft_proofer.hpp"

namespace print_soft_proofer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Print Soft Proofer (Photography and cinematography). Awaits wire-up.";
    return s;
}

}
