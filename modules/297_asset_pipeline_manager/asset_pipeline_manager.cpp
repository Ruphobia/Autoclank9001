#include "asset_pipeline_manager.hpp"

namespace asset_pipeline_manager {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Asset Pipeline Manager (Game development). Awaits wire-up.";
    return s;
}

}
