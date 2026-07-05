#include "document_converter.hpp"

namespace document_converter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Document converter (Documents and reading). Awaits wire-up.";
    return s;
}

}
