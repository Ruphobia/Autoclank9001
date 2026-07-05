#include "test_runner.hpp"
#include "../modules/827_terminology_manager/terminology_manager.hpp"

namespace {

testing::TestOutcome run_terminology_manager() {
    terminology_manager::init();
    auto s = terminology_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    terminology_manager::shutdown();
    return testing::ok();
}

const int _reg_terminology_manager = testing::register_test(
    "terminology_manager",
    "827_terminology_manager: stub status check",
    &run_terminology_manager);

}
