#include "test_runner.hpp"
#include "../modules/2101_anchor_watch/anchor_watch.hpp"

namespace {

testing::TestOutcome run_anchor_watch() {
    anchor_watch::init();
    auto s = anchor_watch::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    anchor_watch::shutdown();
    return testing::ok();
}

const int _reg_anchor_watch = testing::register_test(
    "anchor_watch",
    "2101_anchor_watch: stub status check",
    &run_anchor_watch);

}
