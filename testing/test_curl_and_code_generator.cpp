#include "test_runner.hpp"
#include "../modules/156_curl_and_code_generator/curl_and_code_generator.hpp"

namespace {

testing::TestOutcome run_curl_and_code_generator() {
    curl_and_code_generator::init();
    auto s = curl_and_code_generator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    curl_and_code_generator::shutdown();
    return testing::ok();
}

const int _reg_curl_and_code_generator = testing::register_test(
    "curl_and_code_generator",
    "156_curl_and_code_generator: stub status check",
    &run_curl_and_code_generator);

}
