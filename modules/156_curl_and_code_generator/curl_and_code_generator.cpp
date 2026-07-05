#include "curl_and_code_generator.hpp"

namespace curl_and_code_generator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: cURL and code generator (Web development). Awaits wire-up.";
    return s;
}

}
