#include "etymology_explorer.hpp"

namespace etymology_explorer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Etymology Explorer (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
