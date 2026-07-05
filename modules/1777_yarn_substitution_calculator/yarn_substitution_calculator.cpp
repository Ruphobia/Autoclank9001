#include "yarn_substitution_calculator.hpp"

namespace yarn_substitution_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Yarn Substitution Calculator (Textiles, sewing, soft crafts). Awaits wire-up.";
    return s;
}

}
