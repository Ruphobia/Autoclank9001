#include "word_generator.hpp"

namespace word_generator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Word Generator (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
