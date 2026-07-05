#include "test_runner.hpp"
#include "../modules/1425_qualitative_theme_tagger/qualitative_theme_tagger.hpp"

namespace {

testing::TestOutcome run_qualitative_theme_tagger() {
    qualitative_theme_tagger::init();
    auto s = qualitative_theme_tagger::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    qualitative_theme_tagger::shutdown();
    return testing::ok();
}

const int _reg_qualitative_theme_tagger = testing::register_test(
    "qualitative_theme_tagger",
    "1425_qualitative_theme_tagger: stub status check",
    &run_qualitative_theme_tagger);

}
