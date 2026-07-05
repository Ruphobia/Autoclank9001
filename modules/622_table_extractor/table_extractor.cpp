#include "table_extractor.hpp"

namespace table_extractor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Table extractor (Documents and reading). Awaits wire-up.";
    return s;
}

}
