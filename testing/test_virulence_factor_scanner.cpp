#include "test_runner.hpp"
#include "../modules/1358_virulence_factor_scanner/virulence_factor_scanner.hpp"

namespace {

testing::TestOutcome run_virulence_factor_scanner() {
    virulence_factor_scanner::init();
    auto s = virulence_factor_scanner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    virulence_factor_scanner::shutdown();
    return testing::ok();
}

const int _reg_virulence_factor_scanner = testing::register_test(
    "virulence_factor_scanner",
    "1358_virulence_factor_scanner: stub status check",
    &run_virulence_factor_scanner);

}
