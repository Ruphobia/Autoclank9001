#include "test_runner.hpp"
#include "../modules/887_holography_designer/holography_designer.hpp"

namespace {

testing::TestOutcome run_holography_designer() {
    holography_designer::init();
    auto s = holography_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    holography_designer::shutdown();
    return testing::ok();
}

const int _reg_holography_designer = testing::register_test(
    "holography_designer",
    "887_holography_designer: stub status check",
    &run_holography_designer);

}
