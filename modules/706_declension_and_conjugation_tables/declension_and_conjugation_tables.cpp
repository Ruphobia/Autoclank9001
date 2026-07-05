#include "declension_and_conjugation_tables.hpp"

namespace declension_and_conjugation_tables {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Declension and Conjugation Tables (Ancient languages and paleography). Awaits wire-up.";
    return s;
}

}
