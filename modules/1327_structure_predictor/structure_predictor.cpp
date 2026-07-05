#include "structure_predictor.hpp"

namespace structure_predictor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Structure Predictor (Biology and bioinformatics). Awaits wire-up.";
    return s;
}

}
