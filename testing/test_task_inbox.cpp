#include "test_runner.hpp"
#include "../modules/420_task_inbox/task_inbox.hpp"

namespace {

testing::TestOutcome run_task_inbox() {
    task_inbox::init();
    auto s = task_inbox::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    task_inbox::shutdown();
    return testing::ok();
}

const int _reg_task_inbox = testing::register_test(
    "task_inbox",
    "420_task_inbox: stub status check",
    &run_task_inbox);

}
