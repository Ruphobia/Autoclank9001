#include "test_runner.hpp"
#include "../modules/745_canon_and_apocrypha_tracker/canon_and_apocrypha_tracker.hpp"

namespace {

testing::TestOutcome run_canon_and_apocrypha_tracker() {
    canon_and_apocrypha_tracker::init();
    auto s = canon_and_apocrypha_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    canon_and_apocrypha_tracker::shutdown();
    return testing::ok();
}

const int _reg_canon_and_apocrypha_tracker = testing::register_test(
    "canon_and_apocrypha_tracker",
    "745_canon_and_apocrypha_tracker: stub status check",
    &run_canon_and_apocrypha_tracker);

}
