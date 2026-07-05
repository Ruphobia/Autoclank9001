#include "bundler_config_tuner.hpp"

namespace bundler_config_tuner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Bundler config tuner (Web development). Awaits wire-up.";
    return s;
}

}
