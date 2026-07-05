#include "test_runner.hpp"
#include "../modules/1321_local_blast_console/local_blast_console.hpp"

namespace {

testing::TestOutcome run_local_blast_console() {
    local_blast_console::init();
    auto s = local_blast_console::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    local_blast_console::shutdown();
    return testing::ok();
}

const int _reg_local_blast_console = testing::register_test(
    "local_blast_console",
    "1321_local_blast_console: stub status check",
    &run_local_blast_console);

}
