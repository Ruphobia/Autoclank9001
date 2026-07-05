#include "probate_and_land_parser.hpp"

namespace probate_and_land_parser {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Probate and Land Parser (Detailed genealogy and family history). Awaits wire-up.";
    return s;
}

}
