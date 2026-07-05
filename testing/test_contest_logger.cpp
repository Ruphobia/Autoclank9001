#include "test_runner.hpp"
#include "../modules/1179_contest_logger/contest_logger.hpp"

namespace {

testing::TestOutcome run_contest_logger() {
    contest_logger::init();
    auto s = contest_logger::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    contest_logger::shutdown();
    return testing::ok();
}

const int _reg_contest_logger = testing::register_test(
    "contest_logger",
    "1179_contest_logger: stub status check",
    &run_contest_logger);

}
