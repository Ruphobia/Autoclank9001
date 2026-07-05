#include "test_runner.hpp"
#include "../modules/774_tale_diffusion_mapper/tale_diffusion_mapper.hpp"

namespace {

testing::TestOutcome run_tale_diffusion_mapper() {
    tale_diffusion_mapper::init();
    auto s = tale_diffusion_mapper::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tale_diffusion_mapper::shutdown();
    return testing::ok();
}

const int _reg_tale_diffusion_mapper = testing::register_test(
    "tale_diffusion_mapper",
    "774_tale_diffusion_mapper: stub status check",
    &run_tale_diffusion_mapper);

}
