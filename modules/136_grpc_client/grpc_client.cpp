#include "grpc_client.hpp"

namespace grpc_client {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: gRPC client (Web development). Awaits wire-up.";
    return s;
}

}
