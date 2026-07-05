#include "test_runner.hpp"
#include "../modules/234_backup_and_restore/backup_and_restore.hpp"

namespace {

testing::TestOutcome run_backup_and_restore() {
    backup_and_restore::init();
    auto s = backup_and_restore::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    backup_and_restore::shutdown();
    return testing::ok();
}

const int _reg_backup_and_restore = testing::register_test(
    "backup_and_restore",
    "234_backup_and_restore: stub status check",
    &run_backup_and_restore);

}
