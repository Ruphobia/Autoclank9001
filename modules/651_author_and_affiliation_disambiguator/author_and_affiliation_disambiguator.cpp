#include "author_and_affiliation_disambiguator.hpp"

namespace author_and_affiliation_disambiguator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Author and Affiliation Disambiguator (Academic and scholarly research). Awaits wire-up.";
    return s;
}

}
