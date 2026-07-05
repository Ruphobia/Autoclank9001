#include "test_runner.hpp"
#include "../modules/1709_counterpoint_tutor/counterpoint_tutor.hpp"

namespace {

testing::TestOutcome run_counterpoint_tutor() {
    counterpoint_tutor::init();
    auto s = counterpoint_tutor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    counterpoint_tutor::shutdown();
    return testing::ok();
}

const int _reg_counterpoint_tutor = testing::register_test(
    "counterpoint_tutor",
    "1709_counterpoint_tutor: stub status check",
    &run_counterpoint_tutor);

}
