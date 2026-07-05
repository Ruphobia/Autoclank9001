#include "test_runner.hpp"
#include "../modules/1816_tempeh_and_natto_incubator_log/tempeh_and_natto_incubator_log.hpp"

namespace {

testing::TestOutcome run_tempeh_and_natto_incubator_log() {
    tempeh_and_natto_incubator_log::init();
    auto s = tempeh_and_natto_incubator_log::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tempeh_and_natto_incubator_log::shutdown();
    return testing::ok();
}

const int _reg_tempeh_and_natto_incubator_log = testing::register_test(
    "tempeh_and_natto_incubator_log",
    "1816_tempeh_and_natto_incubator_log: stub status check",
    &run_tempeh_and_natto_incubator_log);

}
