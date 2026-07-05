#include "causal_inference_kit.hpp"

namespace causal_inference_kit {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Causal inference kit (Sociology, political science, public policy). Awaits wire-up.";
    return s;
}

}
