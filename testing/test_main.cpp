#include "test_runner.hpp"

#include <cstdio>
#include <exception>
#include <string>

namespace {

int run_one(const testing::TestCase & t) {
    testing::TestOutcome o;
    try {
        o = t.fn();
    } catch (const std::exception & ex) {
        std::printf("FAIL %s: exception: %s\n", t.name.c_str(), ex.what());
        return 1;
    } catch (...) {
        std::printf("FAIL %s: unknown exception\n", t.name.c_str());
        return 1;
    }
    switch (o.result) {
        case testing::Result::OK:
            std::printf("OK %s\n", t.name.c_str());
            return 0;
        case testing::Result::SKIP:
            std::printf("SKIP %s: %s\n", t.name.c_str(), o.message.c_str());
            return 0;
        case testing::Result::FAIL:
            std::printf("FAIL %s: %s\n", t.name.c_str(), o.message.c_str());
            return 1;
    }
    return 1;
}

void print_usage() {
    std::printf(
        "tool_test: standalone smoke-test runner for the tool modules.\n"
        "\n"
        "Usage:\n"
        "  tool_test --list              list every registered test\n"
        "  tool_test --all               run every registered test\n"
        "  tool_test <name>              run a single test by name\n"
        "  tool_test --feature <name>    alias for <name>\n");
}

const testing::TestCase * find(const std::string & n) {
    for (const auto & t : testing::registry()) {
        if (t.name == n) return &t;
    }
    return nullptr;
}

}

int main(int argc, char ** argv) {
    if (argc < 2) { print_usage(); return 2; }
    const std::string a = argv[1];

    if (a == "--help" || a == "-h") { print_usage(); return 0; }

    if (a == "--list") {
        for (const auto & t : testing::registry()) {
            std::printf("%s\t%s\n", t.name.c_str(), t.description.c_str());
        }
        return 0;
    }

    if (a == "--all") {
        int failures = 0;
        int total    = 0;
        for (const auto & t : testing::registry()) {
            total++;
            if (run_one(t) != 0) failures++;
        }
        std::printf("\n%d/%d passed, %d failed\n",
                    total - failures, total, failures);
        return failures == 0 ? 0 : 1;
    }

    std::string name;
    if (a == "--feature") {
        if (argc < 3) { print_usage(); return 2; }
        name = argv[2];
    } else {
        name = a;
    }

    const auto * t = find(name);
    if (!t) {
        std::printf("FAIL %s: no such test\n", name.c_str());
        return 2;
    }
    return run_one(*t);
}
