#include "test_runner.hpp"
#include "../modules/704_polyglot_lexicon/polyglot_lexicon.hpp"

namespace {

testing::TestOutcome run_polyglot_lexicon() {
    polyglot_lexicon::init();
    auto s = polyglot_lexicon::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    polyglot_lexicon::shutdown();
    return testing::ok();
}

const int _reg_polyglot_lexicon = testing::register_test(
    "polyglot_lexicon",
    "704_polyglot_lexicon: stub status check",
    &run_polyglot_lexicon);

}
