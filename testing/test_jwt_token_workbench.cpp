// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/395_jwt_token_workbench/jwt_token_workbench.hpp"

namespace {

testing::TestOutcome run_jwt_token_workbench() {
    jwt_token_workbench::init();
    auto s = jwt_token_workbench::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    jwt_token_workbench::shutdown();
    return testing::ok();
}

const int _reg_jwt_token_workbench = testing::register_test(
    "jwt_token_workbench",
    "395_jwt_token_workbench: stub status check",
    &run_jwt_token_workbench);

}
