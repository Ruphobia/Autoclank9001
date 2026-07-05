#include "finite_state_lab.hpp"

namespace finite_state_lab {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Finite-State Lab (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
