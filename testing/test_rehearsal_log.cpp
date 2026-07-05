#include "test_runner.hpp"
#include "../modules/1936_rehearsal_log/rehearsal_log.hpp"

namespace {

testing::TestOutcome run_rehearsal_log() {
    rehearsal_log::init();
    auto s = rehearsal_log::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    rehearsal_log::shutdown();
    return testing::ok();
}

const int _reg_rehearsal_log = testing::register_test(
    "rehearsal_log",
    "1936_rehearsal_log: stub status check",
    &run_rehearsal_log);

}
