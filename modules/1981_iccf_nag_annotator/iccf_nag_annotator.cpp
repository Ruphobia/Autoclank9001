#include "iccf_nag_annotator.hpp"

namespace iccf_nag_annotator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: ICCF/NAG annotator (Game playing assistants). Awaits wire-up.";
    return s;
}

}
