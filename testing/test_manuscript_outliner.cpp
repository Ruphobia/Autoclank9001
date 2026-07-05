#include "test_runner.hpp"
#include "../modules/496_manuscript_outliner/manuscript_outliner.hpp"

namespace {

testing::TestOutcome run_manuscript_outliner() {
    manuscript_outliner::init();
    auto s = manuscript_outliner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    manuscript_outliner::shutdown();
    return testing::ok();
}

const int _reg_manuscript_outliner = testing::register_test(
    "manuscript_outliner",
    "496_manuscript_outliner: stub status check",
    &run_manuscript_outliner);

}
