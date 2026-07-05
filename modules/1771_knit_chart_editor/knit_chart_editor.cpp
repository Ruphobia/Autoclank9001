#include "knit_chart_editor.hpp"

namespace knit_chart_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Knit Chart Editor (Textiles, sewing, soft crafts). Awaits wire-up.";
    return s;
}

}
