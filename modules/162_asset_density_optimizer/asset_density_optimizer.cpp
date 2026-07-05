#include "asset_density_optimizer.hpp"

namespace asset_density_optimizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Asset density optimizer (Mobile development). Awaits wire-up.";
    return s;
}

}
