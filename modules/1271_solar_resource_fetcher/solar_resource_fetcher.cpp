#include "solar_resource_fetcher.hpp"

namespace solar_resource_fetcher {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Solar Resource Fetcher (Energy systems). Awaits wire-up.";
    return s;
}

}
