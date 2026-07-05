#include "connection_manager.hpp"

namespace connection_manager {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Connection manager (Databases). Awaits wire-up.";
    return s;
}

}
