#include "campaign_log.hpp"

namespace campaign_log {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Campaign Log (Tabletop RPG and world building). Awaits wire-up.";
    return s;
}

}
