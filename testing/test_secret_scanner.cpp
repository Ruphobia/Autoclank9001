// Smoke test for 069_secret_scanner.

#include "test_runner.hpp"
#include "../modules/069_secret_scanner/secret_scanner.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>

namespace {

namespace fs = std::filesystem;

fs::path scratch_root() {
    if (const char * e = std::getenv("TMPDIR")) return fs::path(e) / "tool_test_secret";
    return fs::path("/tmp") / "tool_test_secret";
}

void write(const fs::path & p, const std::string & body) {
    fs::create_directories(p.parent_path());
    std::ofstream o(p, std::ios::binary);
    o << body;
}

testing::TestOutcome run() {
    fs::path root = scratch_root();
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root);

    secret_scanner::init();

    // High-entropy random-looking string should clear the floor.
    if (secret_scanner::shannon_entropy("aaaaaaaa") > 1.0)
        return testing::fail("entropy of constant > 1");
    if (secret_scanner::shannon_entropy("aBcDeFgHiJkLmNoPqRsTuVwXyZ012345") < 4.5)
        return testing::fail("entropy of random-ish string too low");

    write(root / "src" / "config.py",
        "AWS_KEY = \"AKIAIOSFODNN7EXAMPLE\"\n"
        "GITHUB_TOKEN = \"ghp_" + std::string(36, 'a') + "\"\n"
        "\n"
        "harmless = 'hello'\n"
        "BIG = '" + std::string(48, 'x') + "'   # low entropy, should NOT fire\n"
    );
    write(root / "src" / "key.pem",
        "-----BEGIN RSA PRIVATE KEY-----\n"
        "MIIBOgIBAAJBAKx...redacted...\n"
        "-----END RSA PRIVATE KEY-----\n");
    write(root / "docs" / "notes.md",
        "Some normal documentation. Includes a token-like literal:\n"
        "\"aB3xqDF9aZ2eK8nS7vJ1qP0wL4cR6yT5\"\n"); // high-entropy 32-char

    auto found = secret_scanner::scan(root.string());
    bool saw_aws = false, saw_pem = false, saw_gh = false, saw_high = false;
    for (const auto & f : found) {
        if (f.kind == "aws-access-key"     && f.path.find("config.py") != std::string::npos) saw_aws  = true;
        if (f.kind == "private-key-pem"    && f.path.find("key.pem")   != std::string::npos) saw_pem  = true;
        if (f.kind == "github-pat"         && f.path.find("config.py") != std::string::npos) saw_gh   = true;
        if (f.kind == "high-entropy-string" && f.path.find("notes.md") != std::string::npos) saw_high = true;
        if (f.excerpt.find("AKIAIOSFODNN7EXAMPLE") != std::string::npos)
            return testing::fail("AWS key not redacted");
        if (f.confidence <= 0.0 || f.confidence > 1.0)
            return testing::fail("confidence out of range");
    }
    if (!saw_aws) return testing::fail("AWS key not surfaced");
    if (!saw_pem) return testing::fail("PEM key not surfaced");
    if (!saw_gh)  return testing::fail("GitHub PAT not surfaced");
    if (!saw_high) return testing::fail("high-entropy string not surfaced");

    // No false positives on the all-'x' low-entropy literal.
    for (const auto & f : found) {
        if (f.kind == "high-entropy-string" &&
            f.path.find("config.py") != std::string::npos)
            return testing::fail("low-entropy literal falsely flagged");
    }

    secret_scanner::shutdown();
    fs::remove_all(root, ec);
    return testing::ok();
}

const int _r = testing::register_test(
    "secret_scanner",
    "069_secret_scanner: AWS / GitHub / PEM / high-entropy with redaction",
    &run);

}
