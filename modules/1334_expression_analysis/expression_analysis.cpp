#include "expression_analysis.hpp"

namespace expression_analysis {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Expression Analysis (Biology and bioinformatics). Awaits wire-up.";
    return s;
}

}
