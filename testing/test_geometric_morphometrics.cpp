#include "test_runner.hpp"
#include "../modules/1527_geometric_morphometrics/geometric_morphometrics.hpp"

namespace {

testing::TestOutcome run_geometric_morphometrics() {
    geometric_morphometrics::init();
    auto s = geometric_morphometrics::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    geometric_morphometrics::shutdown();
    return testing::ok();
}

const int _reg_geometric_morphometrics = testing::register_test(
    "geometric_morphometrics",
    "1527_geometric_morphometrics: stub status check",
    &run_geometric_morphometrics);

}
