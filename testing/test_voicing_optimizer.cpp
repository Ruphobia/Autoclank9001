#include "test_runner.hpp"
#include "../modules/1712_voicing_optimizer/voicing_optimizer.hpp"

namespace {

testing::TestOutcome run_voicing_optimizer() {
    voicing_optimizer::init();
    auto s = voicing_optimizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    voicing_optimizer::shutdown();
    return testing::ok();
}

const int _reg_voicing_optimizer = testing::register_test(
    "voicing_optimizer",
    "1712_voicing_optimizer: stub status check",
    &run_voicing_optimizer);

}
