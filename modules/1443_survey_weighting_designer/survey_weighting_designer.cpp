#include "survey_weighting_designer.hpp"

namespace survey_weighting_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Survey weighting designer (Sociology, political science, public policy). Awaits wire-up.";
    return s;
}

}
