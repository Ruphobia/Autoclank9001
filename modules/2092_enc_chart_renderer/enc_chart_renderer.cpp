#include "enc_chart_renderer.hpp"

namespace enc_chart_renderer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: ENC chart renderer (Sailing, boating, navigation). Awaits wire-up.";
    return s;
}

}
