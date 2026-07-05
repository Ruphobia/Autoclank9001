#include "test_runner.hpp"
#include "../modules/490_container_loader/container_loader.hpp"

namespace {

testing::TestOutcome run_container_loader() {
    container_loader::init();
    auto s = container_loader::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    container_loader::shutdown();
    return testing::ok();
}

const int _reg_container_loader = testing::register_test(
    "container_loader",
    "490_container_loader: stub status check",
    &run_container_loader);

}
