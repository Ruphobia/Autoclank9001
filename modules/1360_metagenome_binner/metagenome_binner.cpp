#include "metagenome_binner.hpp"

namespace metagenome_binner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Metagenome binner (Microbiology, virology, and immunology). Awaits wire-up.";
    return s;
}

}
