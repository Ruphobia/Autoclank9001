#include "format_converter.hpp"

namespace format_converter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Format converter (Image and video). Awaits wire-up.";
    return s;
}

}
