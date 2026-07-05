#include "crochet_chart_editor.hpp"

namespace crochet_chart_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Crochet Chart Editor (Textiles, sewing, soft crafts). Awaits wire-up.";
    return s;
}

}
