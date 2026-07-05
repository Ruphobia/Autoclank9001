#include "test_runner.hpp"
#include "../modules/423_email_client/email_client.hpp"

namespace {

testing::TestOutcome run_email_client() {
    email_client::init();
    auto s = email_client::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    email_client::shutdown();
    return testing::ok();
}

const int _reg_email_client = testing::register_test(
    "email_client",
    "423_email_client: stub status check",
    &run_email_client);

}
