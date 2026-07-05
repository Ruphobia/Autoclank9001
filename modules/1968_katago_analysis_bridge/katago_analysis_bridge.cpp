#include "katago_analysis_bridge.hpp"

namespace katago_analysis_bridge {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: KataGo analysis bridge (Game playing assistants). Awaits wire-up.";
    return s;
}

}
