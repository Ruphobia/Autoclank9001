#include "datasheet_reader.hpp"

namespace datasheet_reader {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Datasheet reader (Electronics and EDA). Awaits wire-up.";
    return s;
}

}
