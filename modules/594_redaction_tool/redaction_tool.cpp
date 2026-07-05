#include "redaction_tool.hpp"

namespace redaction_tool {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Redaction Tool (Legal, patents, trademarks, compliance). Awaits wire-up.";
    return s;
}

}
