#include "diversity_index_calculator.hpp"

namespace diversity_index_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Diversity index calculator (Ecology, conservation, sustainability). Awaits wire-up.";
    return s;
}

}
