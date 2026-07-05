#include "statistics_workbench.hpp"

namespace statistics_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Statistics workbench (Physics and math). Awaits wire-up.";
    return s;
}

}
