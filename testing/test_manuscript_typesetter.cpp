#include "test_runner.hpp"
#include "../modules/510_manuscript_typesetter/manuscript_typesetter.hpp"

namespace {

testing::TestOutcome run_manuscript_typesetter() {
    manuscript_typesetter::init();
    auto s = manuscript_typesetter::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    manuscript_typesetter::shutdown();
    return testing::ok();
}

const int _reg_manuscript_typesetter = testing::register_test(
    "manuscript_typesetter",
    "510_manuscript_typesetter: stub status check",
    &run_manuscript_typesetter);

}
