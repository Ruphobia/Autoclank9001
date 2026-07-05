#include "forward_reaction_predictor.hpp"

namespace forward_reaction_predictor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Forward reaction predictor (Chemistry and lab). Awaits wire-up.";
    return s;
}

}
