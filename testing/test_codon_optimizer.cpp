#include "test_runner.hpp"
#include "../modules/1332_codon_optimizer/codon_optimizer.hpp"

namespace {

testing::TestOutcome run_codon_optimizer() {
    codon_optimizer::init();
    auto s = codon_optimizer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    codon_optimizer::shutdown();
    return testing::ok();
}

const int _reg_codon_optimizer = testing::register_test(
    "codon_optimizer",
    "1332_codon_optimizer: stub status check",
    &run_codon_optimizer);

}
