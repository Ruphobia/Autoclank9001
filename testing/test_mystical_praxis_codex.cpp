#include "test_runner.hpp"
#include "../modules/748_mystical_praxis_codex/mystical_praxis_codex.hpp"

namespace {

testing::TestOutcome run_mystical_praxis_codex() {
    mystical_praxis_codex::init();
    auto s = mystical_praxis_codex::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    mystical_praxis_codex::shutdown();
    return testing::ok();
}

const int _reg_mystical_praxis_codex = testing::register_test(
    "mystical_praxis_codex",
    "748_mystical_praxis_codex: stub status check",
    &run_mystical_praxis_codex);

}
