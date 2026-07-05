#include "conworld_linguistic_atlas.hpp"

namespace conworld_linguistic_atlas {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Conworld Linguistic Atlas (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
