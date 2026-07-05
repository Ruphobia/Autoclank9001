#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <nlohmann/json.hpp>

// SBOM and license auditor: builds CycloneDX and SPDX bills of
// materials from a Component list and classifies declared license
// strings against the project's licensing intent. Intent levels
// match the memory note for `tool`: PublicDomain / Permissive /
// WeakCopyleft / StrongCopyleft / Restricted / Unknown.
namespace sbom_auditor {

enum class LicenseClass {
    PublicDomain  = 0,
    Permissive    = 1,
    WeakCopyleft  = 2,
    StrongCopyleft= 3,
    Restricted    = 4,
    Unknown       = 5,
};
const char * to_string(LicenseClass);

struct Component {
    std::string  name;
    std::string  version;
    std::string  type;          // "library" / "application" / "framework" / "operating-system"
    std::string  purl;          // pkg:type/name@version (optional)
    std::string  license;       // SPDX identifier preferred
    std::string  supplier;      // optional human-facing name
};

void init();
void shutdown();

// Classify a license string. Recognizes the common SPDX identifiers
// and a few human-facing aliases (Unlicense, CC0, public-domain,
// MIT, Apache-2.0, BSD-* family, ISC, BlueOak; LGPL, MPL; GPL, AGPL;
// commercial/proprietary). Anything else returns Unknown.
LicenseClass classify_license(std::string_view license);

// Returns the most restrictive class found across `components`.
LicenseClass worst_of(const std::vector<Component> & components);

struct AuditFinding {
    Component   component;
    LicenseClass found;
    std::string reason;        // "stricter than intent", "unknown license", ...
};

struct AuditReport {
    LicenseClass               intent;
    std::vector<AuditFinding>  warnings;
    bool                       all_clear = true;
};

// Compare every component license against `intent` and surface the
// ones that are stricter (or unknown). all_clear is true iff no
// component in the input is more restrictive than the project intent.
AuditReport audit(const std::vector<Component> & components,
                  LicenseClass intent = LicenseClass::Permissive);

// Render a CycloneDX 1.5 JSON SBOM. `project_name` and `project_version`
// describe the root metadata.component entry.
std::string emit_cyclonedx_json(const std::vector<Component> & components,
                                std::string_view project_name,
                                std::string_view project_version);

// Render an SPDX 2.3 JSON SBOM with a minimal-but-spec-shaped header.
std::string emit_spdx_json(const std::vector<Component> & components,
                           std::string_view project_name,
                           std::string_view project_version);

}
