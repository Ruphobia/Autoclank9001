#include "test_runner.hpp"
#include "../modules/796_forced_aligner/forced_aligner.hpp"

namespace {

testing::TestOutcome run_forced_aligner() {
    forced_aligner::init();
    auto s = forced_aligner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    forced_aligner::shutdown();
    return testing::ok();
}

const int _reg_forced_aligner = testing::register_test(
    "forced_aligner",
    "796_forced_aligner: stub status check",
    &run_forced_aligner);

}
