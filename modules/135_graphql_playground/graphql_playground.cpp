#include "graphql_playground.hpp"

namespace graphql_playground {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: GraphQL playground (Web development). Awaits wire-up.";
    return s;
}

}
