#include "test_runner.hpp"
#include "../modules/1925_costume_and_wig_tracker/costume_and_wig_tracker.hpp"

namespace {

testing::TestOutcome run_costume_and_wig_tracker() {
    costume_and_wig_tracker::init();
    auto s = costume_and_wig_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    costume_and_wig_tracker::shutdown();
    return testing::ok();
}

const int _reg_costume_and_wig_tracker = testing::register_test(
    "costume_and_wig_tracker",
    "1925_costume_and_wig_tracker: stub status check",
    &run_costume_and_wig_tracker);

}
