#include "test_runner.hpp"
#include "../modules/116_bundler_config_tuner/bundler_config_tuner.hpp"

namespace {

testing::TestOutcome run_bundler_config_tuner() {
    bundler_config_tuner::init();
    auto s = bundler_config_tuner::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bundler_config_tuner::shutdown();
    return testing::ok();
}

const int _reg_bundler_config_tuner = testing::register_test(
    "bundler_config_tuner",
    "116_bundler_config_tuner: stub status check",
    &run_bundler_config_tuner);

}
