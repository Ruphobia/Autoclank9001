#include "localization_manager.hpp"

namespace localization_manager {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Localization Manager (Game development). Awaits wire-up.";
    return s;
}

}
