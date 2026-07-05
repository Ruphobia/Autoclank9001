#include "glossary_and_index_builder.hpp"

namespace glossary_and_index_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Glossary and index builder (Documents and reading). Awaits wire-up.";
    return s;
}

}
