#include "test_runner.hpp"
#include "../modules/471_doe_designer/doe_designer.hpp"

namespace {

testing::TestOutcome run_doe_designer() {
    doe_designer::init();
    auto s = doe_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    doe_designer::shutdown();
    return testing::ok();
}

const int _reg_doe_designer = testing::register_test(
    "doe_designer",
    "471_doe_designer: stub status check",
    &run_doe_designer);

}
