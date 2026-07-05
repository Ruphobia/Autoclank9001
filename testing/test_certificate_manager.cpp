#include "test_runner.hpp"
#include "../modules/277_certificate_manager/certificate_manager.hpp"

namespace {

testing::TestOutcome run_certificate_manager() {
    certificate_manager::init();
    auto s = certificate_manager::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    certificate_manager::shutdown();
    return testing::ok();
}

const int _reg_certificate_manager = testing::register_test(
    "certificate_manager",
    "277_certificate_manager: stub status check",
    &run_certificate_manager);

}
