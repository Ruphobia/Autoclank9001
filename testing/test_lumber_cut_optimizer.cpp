#include "test_runner.hpp"
#include "../modules/2036_lumber_cut_optimizer/lumber_cut_optimizer.hpp"

namespace {

testing::TestOutcome run_lumber_cut_optimizer() {
    lumber_cut_optimizer::init();
    auto s = lumber_cut_optimizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lumber_cut_optimizer::shutdown();
    return testing::ok();
}

const int _reg_lumber_cut_optimizer = testing::register_test(
    "lumber_cut_optimizer",
    "2036_lumber_cut_optimizer: stub status check",
    &run_lumber_cut_optimizer);

}
