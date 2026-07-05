#include "test_runner.hpp"
#include "../modules/1297_name_and_identifier_translator/name_and_identifier_translator.hpp"

namespace {

testing::TestOutcome run_name_and_identifier_translator() {
    name_and_identifier_translator::init();
    auto s = name_and_identifier_translator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    name_and_identifier_translator::shutdown();
    return testing::ok();
}

const int _reg_name_and_identifier_translator = testing::register_test(
    "name_and_identifier_translator",
    "1297_name_and_identifier_translator: stub status check",
    &run_name_and_identifier_translator);

}
