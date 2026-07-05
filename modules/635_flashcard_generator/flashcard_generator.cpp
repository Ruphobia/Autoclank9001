#include "flashcard_generator.hpp"

namespace flashcard_generator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Flashcard generator (Documents and reading). Awaits wire-up.";
    return s;
}

}
