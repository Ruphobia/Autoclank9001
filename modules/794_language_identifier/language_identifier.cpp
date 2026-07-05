#include "language_identifier.hpp"

namespace language_identifier {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Language Identifier (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
