#include "read_mapper.hpp"

namespace read_mapper {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Read mapper (Microbiology, virology, and immunology). Awaits wire-up.";
    return s;
}

}
