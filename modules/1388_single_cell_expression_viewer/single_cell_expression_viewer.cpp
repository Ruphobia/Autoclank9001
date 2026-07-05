#include "single_cell_expression_viewer.hpp"

namespace single_cell_expression_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Single-cell expression viewer (Genomics, synthetic biology, and neuroscience). Awaits wire-up.";
    return s;
}

}
