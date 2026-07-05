#include "test_runner.hpp"
#include "../modules/1938_illusion_blueprint_workshop/illusion_blueprint_workshop.hpp"

namespace {

testing::TestOutcome run_illusion_blueprint_workshop() {
    illusion_blueprint_workshop::init();
    auto s = illusion_blueprint_workshop::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    illusion_blueprint_workshop::shutdown();
    return testing::ok();
}

const int _reg_illusion_blueprint_workshop = testing::register_test(
    "illusion_blueprint_workshop",
    "1938_illusion_blueprint_workshop: stub status check",
    &run_illusion_blueprint_workshop);

}
