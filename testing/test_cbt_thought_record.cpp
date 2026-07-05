#include "test_runner.hpp"
#include "../modules/2173_cbt_thought_record/cbt_thought_record.hpp"

namespace {

testing::TestOutcome run_cbt_thought_record() {
    cbt_thought_record::init();
    auto s = cbt_thought_record::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cbt_thought_record::shutdown();
    return testing::ok();
}

const int _reg_cbt_thought_record = testing::register_test(
    "cbt_thought_record",
    "2173_cbt_thought_record: stub status check",
    &run_cbt_thought_record);

}
