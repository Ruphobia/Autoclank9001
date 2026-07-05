#include "test_runner.hpp"
#include "../modules/2004_hive_inspection_log/hive_inspection_log.hpp"

namespace {

testing::TestOutcome run_hive_inspection_log() {
    hive_inspection_log::init();
    auto s = hive_inspection_log::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    hive_inspection_log::shutdown();
    return testing::ok();
}

const int _reg_hive_inspection_log = testing::register_test(
    "hive_inspection_log",
    "2004_hive_inspection_log: stub status check",
    &run_hive_inspection_log);

}
