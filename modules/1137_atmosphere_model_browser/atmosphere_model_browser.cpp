#include "atmosphere_model_browser.hpp"

namespace atmosphere_model_browser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Atmosphere Model Browser (Aerospace and orbital mechanics). Awaits wire-up.";
    return s;
}

}
