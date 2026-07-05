#include "test_runner.hpp"
#include "../modules/862_antenna_designer/antenna_designer.hpp"

namespace {

testing::TestOutcome run_antenna_designer() {
    antenna_designer::init();
    auto s = antenna_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    antenna_designer::shutdown();
    return testing::ok();
}

const int _reg_antenna_designer = testing::register_test(
    "antenna_designer",
    "862_antenna_designer: stub status check",
    &run_antenna_designer);

}
