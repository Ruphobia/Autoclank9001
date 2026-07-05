#include "test_runner.hpp"
#include "../modules/474_jsa_and_permit_to_work_builder/jsa_and_permit_to_work_builder.hpp"

namespace {

testing::TestOutcome run_jsa_and_permit_to_work_builder() {
    jsa_and_permit_to_work_builder::init();
    auto s = jsa_and_permit_to_work_builder::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    jsa_and_permit_to_work_builder::shutdown();
    return testing::ok();
}

const int _reg_jsa_and_permit_to_work_builder = testing::register_test(
    "jsa_and_permit_to_work_builder",
    "474_jsa_and_permit_to_work_builder: stub status check",
    &run_jsa_and_permit_to_work_builder);

}
