#include "test_runner.hpp"
#include "../modules/1806_lacto_fermentation_acidity_tracker/lacto_fermentation_acidity_tracker.hpp"

namespace {

testing::TestOutcome run_lacto_fermentation_acidity_tracker() {
    lacto_fermentation_acidity_tracker::init();
    auto s = lacto_fermentation_acidity_tracker::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    lacto_fermentation_acidity_tracker::shutdown();
    return testing::ok();
}

const int _reg_lacto_fermentation_acidity_tracker = testing::register_test(
    "lacto_fermentation_acidity_tracker",
    "1806_lacto_fermentation_acidity_tracker: stub status check",
    &run_lacto_fermentation_acidity_tracker);

}
