#include "syntax_tree_builder.hpp"

namespace syntax_tree_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Syntax Tree Builder (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
