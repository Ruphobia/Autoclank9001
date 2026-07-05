#include "policy_citation_manager.hpp"

namespace policy_citation_manager {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Policy citation manager (Sociology, political science, public policy). Awaits wire-up.";
    return s;
}

}
