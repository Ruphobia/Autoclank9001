#include "terms_of_service_builder.hpp"

namespace terms_of_service_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Terms of Service Builder (Legal, patents, trademarks, compliance). Awaits wire-up.";
    return s;
}

}
