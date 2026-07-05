#include "test_runner.hpp"
#include "../modules/832_romanization_and_transliteration/romanization_and_transliteration.hpp"

namespace {

testing::TestOutcome run_romanization_and_transliteration() {
    romanization_and_transliteration::init();
    auto s = romanization_and_transliteration::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    romanization_and_transliteration::shutdown();
    return testing::ok();
}

const int _reg_romanization_and_transliteration = testing::register_test(
    "romanization_and_transliteration",
    "832_romanization_and_transliteration: stub status check",
    &run_romanization_and_transliteration);

}
