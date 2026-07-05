#include "test_runner.hpp"
#include "../modules/755_concordance_and_kwic/concordance_and_kwic.hpp"

namespace {

testing::TestOutcome run_concordance_and_kwic() {
    concordance_and_kwic::init();
    auto s = concordance_and_kwic::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    concordance_and_kwic::shutdown();
    return testing::ok();
}

const int _reg_concordance_and_kwic = testing::register_test(
    "concordance_and_kwic",
    "755_concordance_and_kwic: stub status check",
    &run_concordance_and_kwic);

}
