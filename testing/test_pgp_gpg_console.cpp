// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/379_pgp_gpg_console/pgp_gpg_console.hpp"

namespace {

testing::TestOutcome run_pgp_gpg_console() {
    pgp_gpg_console::init();
    auto s = pgp_gpg_console::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    pgp_gpg_console::shutdown();
    return testing::ok();
}

const int _reg_pgp_gpg_console = testing::register_test(
    "pgp_gpg_console",
    "379_pgp_gpg_console: stub status check",
    &run_pgp_gpg_console);

}
