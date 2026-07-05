#include "dependency_manager.hpp"

namespace dependency_manager {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Dependency manager (Software development). Awaits wire-up.";
    return s;
}

}
