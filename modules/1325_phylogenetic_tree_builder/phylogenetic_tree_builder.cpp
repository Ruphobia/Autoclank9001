#include "phylogenetic_tree_builder.hpp"

namespace phylogenetic_tree_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Phylogenetic Tree Builder (Biology and bioinformatics). Awaits wire-up.";
    return s;
}

}
