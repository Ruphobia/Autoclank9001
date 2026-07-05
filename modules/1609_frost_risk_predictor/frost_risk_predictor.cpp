#include "frost_risk_predictor.hpp"

namespace frost_risk_predictor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Frost Risk Predictor (Agriculture and horticulture). Awaits wire-up.";
    return s;
}

}
