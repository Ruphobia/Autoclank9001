#include "test_runner.hpp"
#include "../modules/871_non_sequential_ray_tracer/non_sequential_ray_tracer.hpp"

namespace {

testing::TestOutcome run_non_sequential_ray_tracer() {
    non_sequential_ray_tracer::init();
    auto s = non_sequential_ray_tracer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    non_sequential_ray_tracer::shutdown();
    return testing::ok();
}

const int _reg_non_sequential_ray_tracer = testing::register_test(
    "non_sequential_ray_tracer",
    "871_non_sequential_ray_tracer: stub status check",
    &run_non_sequential_ray_tracer);

}
