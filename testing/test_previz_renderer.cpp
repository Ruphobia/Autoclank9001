#include "test_runner.hpp"
#include "../modules/1914_previz_renderer/previz_renderer.hpp"

namespace {

testing::TestOutcome run_previz_renderer() {
    previz_renderer::init();
    auto s = previz_renderer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    previz_renderer::shutdown();
    return testing::ok();
}

const int _reg_previz_renderer = testing::register_test(
    "previz_renderer",
    "1914_previz_renderer: stub status check",
    &run_previz_renderer);

}
