#include "test_runner.hpp"
#include "../modules/511_ebook_compiler/ebook_compiler.hpp"

namespace {

testing::TestOutcome run_ebook_compiler() {
    ebook_compiler::init();
    auto s = ebook_compiler::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ebook_compiler::shutdown();
    return testing::ok();
}

const int _reg_ebook_compiler = testing::register_test(
    "ebook_compiler",
    "511_ebook_compiler: stub status check",
    &run_ebook_compiler);

}
