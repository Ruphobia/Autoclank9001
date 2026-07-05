#include "signalk_schema_bridge.hpp"

namespace signalk_schema_bridge {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: SignalK schema bridge (Sailing, boating, navigation). Awaits wire-up.";
    return s;
}

}
