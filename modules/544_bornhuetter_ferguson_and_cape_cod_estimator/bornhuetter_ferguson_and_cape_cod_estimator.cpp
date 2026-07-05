#include "bornhuetter_ferguson_and_cape_cod_estimator.hpp"

namespace bornhuetter_ferguson_and_cape_cod_estimator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Bornhuetter-Ferguson and Cape Cod estimator (Real estate, insurance, actuarial). Awaits wire-up.";
    return s;
}

}
