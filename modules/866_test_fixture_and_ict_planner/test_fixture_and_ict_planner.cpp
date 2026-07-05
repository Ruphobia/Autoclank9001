#include "test_fixture_and_ict_planner.hpp"

namespace test_fixture_and_ict_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Test fixture and ICT planner (Electronics and EDA). Awaits wire-up.";
    return s;
}

}
