#include "test_runner.hpp"
#include "../modules/1570_citizen_science_submission_helper/citizen_science_submission_helper.hpp"

namespace {

testing::TestOutcome run_citizen_science_submission_helper() {
    citizen_science_submission_helper::init();
    auto s = citizen_science_submission_helper::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    citizen_science_submission_helper::shutdown();
    return testing::ok();
}

const int _reg_citizen_science_submission_helper = testing::register_test(
    "citizen_science_submission_helper",
    "1570_citizen_science_submission_helper: stub status check",
    &run_citizen_science_submission_helper);

}
