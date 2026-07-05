#include "cdc_stream_viewer.hpp"

namespace cdc_stream_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: CDC stream viewer (Databases). Awaits wire-up.";
    return s;
}

}
