#include "test_runner.hpp"
#include "../modules/1185_nec_antenna_designer/nec_antenna_designer.hpp"

namespace {

testing::TestOutcome run_nec_antenna_designer() {
    nec_antenna_designer::init();
    auto s = nec_antenna_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    nec_antenna_designer::shutdown();
    return testing::ok();
}

const int _reg_nec_antenna_designer = testing::register_test(
    "nec_antenna_designer",
    "1185_nec_antenna_designer: stub status check",
    &run_nec_antenna_designer);

}
