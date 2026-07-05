#include "test_runner.hpp"
#include "../modules/2018_nec_wire_and_conduit_sizer/nec_wire_and_conduit_sizer.hpp"

namespace {

testing::TestOutcome run_nec_wire_and_conduit_sizer() {
    nec_wire_and_conduit_sizer::init();
    auto s = nec_wire_and_conduit_sizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    nec_wire_and_conduit_sizer::shutdown();
    return testing::ok();
}

const int _reg_nec_wire_and_conduit_sizer = testing::register_test(
    "nec_wire_and_conduit_sizer",
    "2018_nec_wire_and_conduit_sizer: stub status check",
    &run_nec_wire_and_conduit_sizer);

}
