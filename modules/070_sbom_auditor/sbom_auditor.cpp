#include "sbom_auditor.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <unordered_map>

namespace sbom_auditor {
namespace {

std::string lower(std::string_view s) {
    std::string out(s);
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });
    return out;
}

const std::unordered_map<std::string, LicenseClass> & table() {
    static const std::unordered_map<std::string, LicenseClass> m = {
        // Public domain / equivalents
        {"unlicense",       LicenseClass::PublicDomain},
        {"cc0-1.0",         LicenseClass::PublicDomain},
        {"cc0",             LicenseClass::PublicDomain},
        {"public-domain",   LicenseClass::PublicDomain},
        {"0bsd",            LicenseClass::PublicDomain},
        {"wtfpl",           LicenseClass::PublicDomain},

        // Permissive
        {"mit",             LicenseClass::Permissive},
        {"apache-2.0",      LicenseClass::Permissive},
        {"apache 2",        LicenseClass::Permissive},
        {"bsd-2-clause",    LicenseClass::Permissive},
        {"bsd-3-clause",    LicenseClass::Permissive},
        {"bsd",             LicenseClass::Permissive},
        {"isc",             LicenseClass::Permissive},
        {"blueoak-1.0.0",   LicenseClass::Permissive},
        {"zlib",            LicenseClass::Permissive},
        {"x11",             LicenseClass::Permissive},

        // Weak copyleft
        {"lgpl-2.1-only",   LicenseClass::WeakCopyleft},
        {"lgpl-2.1-or-later", LicenseClass::WeakCopyleft},
        {"lgpl-3.0-only",   LicenseClass::WeakCopyleft},
        {"lgpl-3.0-or-later", LicenseClass::WeakCopyleft},
        {"lgpl",            LicenseClass::WeakCopyleft},
        {"mpl-2.0",         LicenseClass::WeakCopyleft},
        {"epl-2.0",         LicenseClass::WeakCopyleft},
        {"cddl-1.0",        LicenseClass::WeakCopyleft},

        // Strong copyleft
        {"gpl-2.0-only",    LicenseClass::StrongCopyleft},
        {"gpl-2.0-or-later",LicenseClass::StrongCopyleft},
        {"gpl-3.0-only",    LicenseClass::StrongCopyleft},
        {"gpl-3.0-or-later",LicenseClass::StrongCopyleft},
        {"gpl",             LicenseClass::StrongCopyleft},
        {"agpl-3.0-only",   LicenseClass::StrongCopyleft},
        {"agpl-3.0-or-later",LicenseClass::StrongCopyleft},
        {"agpl",            LicenseClass::StrongCopyleft},

        // Restricted / commercial
        {"proprietary",     LicenseClass::Restricted},
        {"commercial",      LicenseClass::Restricted},
        {"noncommercial",   LicenseClass::Restricted},
        {"cc-by-nc",        LicenseClass::Restricted},
        {"cc-by-nc-sa",     LicenseClass::Restricted},
        {"sspl-1.0",        LicenseClass::Restricted},
        {"busl-1.1",        LicenseClass::Restricted},
    };
    return m;
}

}

const char * to_string(LicenseClass c) {
    switch (c) {
        case LicenseClass::PublicDomain:   return "public-domain";
        case LicenseClass::Permissive:     return "permissive";
        case LicenseClass::WeakCopyleft:   return "weak-copyleft";
        case LicenseClass::StrongCopyleft: return "strong-copyleft";
        case LicenseClass::Restricted:     return "restricted";
        case LicenseClass::Unknown:        return "unknown";
    }
    return "unknown";
}

void init()     {}
void shutdown() {}

LicenseClass classify_license(std::string_view license_v) {
    std::string l = lower(license_v);
    auto & t = table();
    auto it = t.find(l);
    if (it != t.end()) return it->second;
    // Substring fallbacks for compound expressions like "MIT OR Apache-2.0".
    if (l.find("agpl") != std::string::npos)            return LicenseClass::StrongCopyleft;
    if (l.find("gpl") != std::string::npos &&
        l.find("lgpl") == std::string::npos)            return LicenseClass::StrongCopyleft;
    if (l.find("lgpl") != std::string::npos)            return LicenseClass::WeakCopyleft;
    if (l.find("mpl-2") != std::string::npos ||
        l.find("epl") != std::string::npos)             return LicenseClass::WeakCopyleft;
    if (l.find("mit") != std::string::npos ||
        l.find("apache") != std::string::npos ||
        l.find("bsd") != std::string::npos ||
        l.find("isc") != std::string::npos)             return LicenseClass::Permissive;
    if (l.find("cc0") != std::string::npos ||
        l.find("unlicense") != std::string::npos ||
        l.find("public domain") != std::string::npos)   return LicenseClass::PublicDomain;
    if (l.find("proprietary") != std::string::npos ||
        l.find("commercial")  != std::string::npos)     return LicenseClass::Restricted;
    return LicenseClass::Unknown;
}

LicenseClass worst_of(const std::vector<Component> & cs) {
    LicenseClass worst = LicenseClass::PublicDomain;
    for (const auto & c : cs) {
        LicenseClass k = classify_license(c.license);
        if ((int)k > (int)worst) worst = k;
    }
    return worst;
}

AuditReport audit(const std::vector<Component> & cs, LicenseClass intent) {
    AuditReport r;
    r.intent = intent;
    for (const auto & c : cs) {
        LicenseClass k = classify_license(c.license);
        if ((int)k > (int)intent) {
            AuditFinding f;
            f.component = c;
            f.found     = k;
            std::ostringstream os;
            os << to_string(k) << " is stricter than intent " << to_string(intent);
            if (k == LicenseClass::Unknown) os << " (unrecognized license: '" << c.license << "')";
            f.reason = os.str();
            r.warnings.push_back(std::move(f));
            r.all_clear = false;
        }
    }
    return r;
}

std::string emit_cyclonedx_json(const std::vector<Component> & cs,
                                std::string_view project_name,
                                std::string_view project_version) {
    nlohmann::json doc;
    doc["bomFormat"]    = "CycloneDX";
    doc["specVersion"]  = "1.5";
    doc["version"]      = 1;
    doc["metadata"]     = {
        {"component", {
            {"type",    "application"},
            {"name",    std::string(project_name)},
            {"version", std::string(project_version)},
        }}
    };
    doc["components"] = nlohmann::json::array();
    for (const auto & c : cs) {
        nlohmann::json j;
        j["type"]    = c.type.empty() ? "library" : c.type;
        j["name"]    = c.name;
        j["version"] = c.version;
        if (!c.purl.empty())     j["purl"]     = c.purl;
        if (!c.supplier.empty()) j["supplier"] = {{"name", c.supplier}};
        if (!c.license.empty()) {
            j["licenses"] = nlohmann::json::array();
            j["licenses"].push_back({{"license", {{"id", c.license}}}});
        }
        doc["components"].push_back(j);
    }
    return doc.dump(2);
}

std::string emit_spdx_json(const std::vector<Component> & cs,
                           std::string_view project_name,
                           std::string_view project_version) {
    nlohmann::json doc;
    doc["spdxVersion"]   = "SPDX-2.3";
    doc["dataLicense"]   = "CC0-1.0";
    doc["SPDXID"]        = "SPDXRef-DOCUMENT";
    doc["name"]          = std::string(project_name);
    doc["documentNamespace"] = std::string("urn:tool-sbom:") + std::string(project_name) +
                               ":" + std::string(project_version);
    doc["creationInfo"]  = {
        {"creators",        nlohmann::json::array({"Tool: tool-sbom-auditor"})},
        {"created",         "1970-01-01T00:00:00Z"}    // placeholder; caller can stamp
    };
    doc["packages"] = nlohmann::json::array();
    doc["packages"].push_back({
        {"name",            std::string(project_name)},
        {"SPDXID",          "SPDXRef-Package-Root"},
        {"versionInfo",     std::string(project_version)},
        {"downloadLocation","NOASSERTION"},
        {"filesAnalyzed",   false},
    });
    int n = 0;
    for (const auto & c : cs) {
        nlohmann::json j;
        j["name"]             = c.name;
        j["SPDXID"]           = "SPDXRef-Package-" + std::to_string(++n);
        j["versionInfo"]      = c.version;
        j["downloadLocation"] = c.purl.empty() ? std::string("NOASSERTION") : c.purl;
        j["licenseDeclared"]  = c.license.empty() ? std::string("NOASSERTION") : c.license;
        j["filesAnalyzed"]    = false;
        if (!c.supplier.empty()) j["supplier"] = std::string("Organization: ") + c.supplier;
        doc["packages"].push_back(j);
    }
    return doc.dump(2);
}

}
