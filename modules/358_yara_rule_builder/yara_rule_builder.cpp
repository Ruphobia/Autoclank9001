#include "yara_rule_builder.hpp"

namespace yara_rule_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: YARA Rule Builder (Cybersecurity and digital forensics). Awaits wire-up.";
    return s;
}

}
