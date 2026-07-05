#include "morphological_parser.hpp"

namespace morphological_parser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Morphological Parser (Ancient languages and paleography). Awaits wire-up.";
    return s;
}

}
