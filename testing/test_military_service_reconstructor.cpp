#include "test_runner.hpp"
#include "../modules/2139_military_service_reconstructor/military_service_reconstructor.hpp"

namespace {

testing::TestOutcome run_military_service_reconstructor() {
    military_service_reconstructor::init();
    auto s = military_service_reconstructor::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    military_service_reconstructor::shutdown();
    return testing::ok();
}

const int _reg_military_service_reconstructor = testing::register_test(
    "military_service_reconstructor",
    "2139_military_service_reconstructor: stub status check",
    &run_military_service_reconstructor);

}
