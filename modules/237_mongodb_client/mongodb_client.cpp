#include "mongodb_client.hpp"

namespace mongodb_client {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: MongoDB client (Databases). Awaits wire-up.";
    return s;
}

}
