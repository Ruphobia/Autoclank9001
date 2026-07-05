#include "test_runner.hpp"
#include "../modules/2141_surname_etymology_and_patronymic_resolver/surname_etymology_and_patronymic_resolver.hpp"

namespace {

testing::TestOutcome run_surname_etymology_and_patronymic_resolver() {
    surname_etymology_and_patronymic_resolver::init();
    auto s = surname_etymology_and_patronymic_resolver::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    surname_etymology_and_patronymic_resolver::shutdown();
    return testing::ok();
}

const int _reg_surname_etymology_and_patronymic_resolver = testing::register_test(
    "surname_etymology_and_patronymic_resolver",
    "2141_surname_etymology_and_patronymic_resolver: stub status check",
    &run_surname_etymology_and_patronymic_resolver);

}
