#include "noise_model_designer.hpp"

namespace noise_model_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Noise Model Designer (Quantum computing). Awaits wire-up.";
    return s;
}

}
