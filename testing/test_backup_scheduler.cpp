#include "test_runner.hpp"
#include "../modules/264_backup_scheduler/backup_scheduler.hpp"

namespace {

testing::TestOutcome run_backup_scheduler() {
    backup_scheduler::init();
    auto s = backup_scheduler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    backup_scheduler::shutdown();
    return testing::ok();
}

const int _reg_backup_scheduler = testing::register_test(
    "backup_scheduler",
    "264_backup_scheduler: stub status check",
    &run_backup_scheduler);

}
