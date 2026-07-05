#include "clay_and_shrinkage_calculator.hpp"

namespace clay_and_shrinkage_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Clay and shrinkage calculator (Visual arts: calligraphy, painting, sculpture). Awaits wire-up.";
    return s;
}

}
