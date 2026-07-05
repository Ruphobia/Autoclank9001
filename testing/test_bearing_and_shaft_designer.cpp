#include "test_runner.hpp"
#include "../modules/937_bearing_and_shaft_designer/bearing_and_shaft_designer.hpp"

namespace {

testing::TestOutcome run_bearing_and_shaft_designer() {
    bearing_and_shaft_designer::init();
    auto s = bearing_and_shaft_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bearing_and_shaft_designer::shutdown();
    return testing::ok();
}

const int _reg_bearing_and_shaft_designer = testing::register_test(
    "bearing_and_shaft_designer",
    "937_bearing_and_shaft_designer: stub status check",
    &run_bearing_and_shaft_designer);

}
