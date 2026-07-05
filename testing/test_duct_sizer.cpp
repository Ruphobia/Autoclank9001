#include "test_runner.hpp"
#include "../modules/1041_duct_sizer/duct_sizer.hpp"

namespace {

testing::TestOutcome run_duct_sizer() {
    duct_sizer::init();
    auto s = duct_sizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    duct_sizer::shutdown();
    return testing::ok();
}

const int _reg_duct_sizer = testing::register_test(
    "duct_sizer",
    "1041_duct_sizer: stub status check",
    &run_duct_sizer);

}
