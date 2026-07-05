#include "argo_float_fetcher.hpp"

namespace argo_float_fetcher {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Argo float fetcher (Oceanography and marine science). Awaits wire-up.";
    return s;
}

}
