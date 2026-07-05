#include "test_runner.hpp"
#include "../modules/432_mail_merge/mail_merge.hpp"

namespace {

testing::TestOutcome run_mail_merge() {
    mail_merge::init();
    auto s = mail_merge::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mail_merge::shutdown();
    return testing::ok();
}

const int _reg_mail_merge = testing::register_test(
    "mail_merge",
    "432_mail_merge: stub status check",
    &run_mail_merge);

}
