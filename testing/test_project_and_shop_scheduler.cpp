#include "test_runner.hpp"
#include "../modules/963_project_and_shop_scheduler/project_and_shop_scheduler.hpp"

namespace {

testing::TestOutcome run_project_and_shop_scheduler() {
    project_and_shop_scheduler::init();
    auto s = project_and_shop_scheduler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    project_and_shop_scheduler::shutdown();
    return testing::ok();
}

const int _reg_project_and_shop_scheduler = testing::register_test(
    "project_and_shop_scheduler",
    "963_project_and_shop_scheduler: stub status check",
    &run_project_and_shop_scheduler);

}
