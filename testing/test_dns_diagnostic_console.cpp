#include "test_runner.hpp"
#include "../modules/273_dns_diagnostic_console/dns_diagnostic_console.hpp"

namespace {

testing::TestOutcome run_dns_diagnostic_console() {
    dns_diagnostic_console::init();
    auto s = dns_diagnostic_console::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dns_diagnostic_console::shutdown();
    return testing::ok();
}

const int _reg_dns_diagnostic_console = testing::register_test(
    "dns_diagnostic_console",
    "273_dns_diagnostic_console: stub status check",
    &run_dns_diagnostic_console);

}
