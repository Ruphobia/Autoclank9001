#include "test_runner.hpp"
#include "../modules/759_comparative_religion_timeline/comparative_religion_timeline.hpp"

namespace {

testing::TestOutcome run_comparative_religion_timeline() {
    comparative_religion_timeline::init();
    auto s = comparative_religion_timeline::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    comparative_religion_timeline::shutdown();
    return testing::ok();
}

const int _reg_comparative_religion_timeline = testing::register_test(
    "comparative_religion_timeline",
    "759_comparative_religion_timeline: stub status check",
    &run_comparative_religion_timeline);

}
