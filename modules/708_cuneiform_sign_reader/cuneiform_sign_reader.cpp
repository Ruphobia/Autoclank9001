#include "cuneiform_sign_reader.hpp"

namespace cuneiform_sign_reader {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Cuneiform Sign Reader (Ancient languages and paleography). Awaits wire-up.";
    return s;
}

}
