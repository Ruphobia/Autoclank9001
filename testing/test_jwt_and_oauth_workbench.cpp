#include "test_runner.hpp"
#include "../modules/151_jwt_and_oauth_workbench/jwt_and_oauth_workbench.hpp"

namespace {

testing::TestOutcome run_jwt_and_oauth_workbench() {
    jwt_and_oauth_workbench::init();
    auto s = jwt_and_oauth_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    jwt_and_oauth_workbench::shutdown();
    return testing::ok();
}

const int _reg_jwt_and_oauth_workbench = testing::register_test(
    "jwt_and_oauth_workbench",
    "151_jwt_and_oauth_workbench: stub status check",
    &run_jwt_and_oauth_workbench);

}
