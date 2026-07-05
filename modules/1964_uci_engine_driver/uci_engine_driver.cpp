#include "uci_engine_driver.hpp"

namespace uci_engine_driver {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: UCI engine driver (Game playing assistants). Awaits wire-up.";
    return s;
}

}
