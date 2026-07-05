// Smoke test for 070_sbom_auditor.

#include "test_runner.hpp"
#include "../modules/070_sbom_auditor/sbom_auditor.hpp"

namespace {

testing::TestOutcome run() {
    sbom_auditor::init();

    using sbom_auditor::LicenseClass;
    using sbom_auditor::Component;

    if (sbom_auditor::classify_license("MIT") != LicenseClass::Permissive)
        return testing::fail("MIT not Permissive");
    if (sbom_auditor::classify_license("Apache-2.0") != LicenseClass::Permissive)
        return testing::fail("Apache-2.0 not Permissive");
    if (sbom_auditor::classify_license("GPL-3.0-or-later") != LicenseClass::StrongCopyleft)
        return testing::fail("GPL not strong");
    if (sbom_auditor::classify_license("LGPL-2.1") != LicenseClass::WeakCopyleft)
        return testing::fail("LGPL not weak");
    if (sbom_auditor::classify_license("Unlicense") != LicenseClass::PublicDomain)
        return testing::fail("Unlicense not PD");
    if (sbom_auditor::classify_license("Proprietary") != LicenseClass::Restricted)
        return testing::fail("proprietary not restricted");
    if (sbom_auditor::classify_license("MIT OR Apache-2.0") != LicenseClass::Permissive)
        return testing::fail("compound MIT OR Apache fallback wrong");
    if (sbom_auditor::classify_license("ZED-2026") != LicenseClass::Unknown)
        return testing::fail("unknown not Unknown");

    std::vector<Component> cs = {
        {"nlohmann_json", "3.11.3",   "library", "pkg:github/nlohmann/json@v3.11.3", "MIT",         "nlohmann"},
        {"libcurl",       "8.10.1",   "library", "pkg:generic/curl@8.10.1",           "MIT",         "Daniel Stenberg"},
        {"openssl",       "3.4.0",    "library", "pkg:generic/openssl@3.4.0",         "Apache-2.0",  "OpenSSL"},
        {"libreadline",   "8.2",      "library", "",                                   "GPL-3.0-or-later", "FSF"},
        {"some_weird",    "0.1",      "library", "",                                   "ZED-2026",    "Unknown Inc."}
    };

    auto rep = sbom_auditor::audit(cs, LicenseClass::Permissive);
    if (rep.all_clear) return testing::fail("audit should not be clear");
    if (rep.warnings.size() != 2)
        return testing::fail("expected 2 warnings (GPL + Unknown), got " +
                             std::to_string(rep.warnings.size()));
    bool saw_gpl = false, saw_unknown = false;
    for (const auto & w : rep.warnings) {
        if (w.found == LicenseClass::StrongCopyleft && w.component.name == "libreadline") saw_gpl = true;
        if (w.found == LicenseClass::Unknown        && w.component.name == "some_weird")  saw_unknown = true;
    }
    if (!saw_gpl || !saw_unknown)
        return testing::fail("expected warnings not surfaced");

    // PD intent: even MIT components are stricter than PD.
    auto strict = sbom_auditor::audit(cs, LicenseClass::PublicDomain);
    if (strict.warnings.size() < 4)
        return testing::fail("PD intent should warn on MIT/Apache too");

    auto cdx = sbom_auditor::emit_cyclonedx_json(cs, "tool", "0.1.0");
    if (cdx.find("\"bomFormat\": \"CycloneDX\"") == std::string::npos)
        return testing::fail("CycloneDX header missing");
    if (cdx.find("\"name\": \"libcurl\"") == std::string::npos)
        return testing::fail("CycloneDX component missing");

    auto spdx = sbom_auditor::emit_spdx_json(cs, "tool", "0.1.0");
    if (spdx.find("\"spdxVersion\": \"SPDX-2.3\"") == std::string::npos)
        return testing::fail("SPDX version header missing");
    if (spdx.find("\"licenseDeclared\": \"GPL-3.0-or-later\"") == std::string::npos)
        return testing::fail("SPDX licenseDeclared missing");

    sbom_auditor::shutdown();
    return testing::ok();
}

const int _r = testing::register_test(
    "sbom_auditor",
    "070_sbom_auditor: classify + audit + CycloneDX + SPDX render",
    &run);

}
