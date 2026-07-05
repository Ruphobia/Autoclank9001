#include "vector_asset_converter.hpp"

namespace vector_asset_converter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Vector asset converter (Mobile development). Awaits wire-up.";
    return s;
}

}
