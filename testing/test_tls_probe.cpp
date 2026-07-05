#include "test_runner.hpp"
#include "../modules/378_tls_probe/tls_probe.hpp"

namespace {

testing::TestOutcome run_tls_probe() {
    tls_probe::init();
    auto s = tls_probe::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tls_probe::shutdown();
    return testing::ok();
}

const int _reg_tls_probe = testing::register_test(
    "tls_probe",
    "378_tls_probe: stub status check",
    &run_tls_probe);

}
