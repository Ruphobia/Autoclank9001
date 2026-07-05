#include "test_runner.hpp"
#include "../modules/836_caption_renderer/caption_renderer.hpp"

namespace {

testing::TestOutcome run_caption_renderer() {
    caption_renderer::init();
    auto s = caption_renderer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    caption_renderer::shutdown();
    return testing::ok();
}

const int _reg_caption_renderer = testing::register_test(
    "caption_renderer",
    "836_caption_renderer: stub status check",
    &run_caption_renderer);

}
