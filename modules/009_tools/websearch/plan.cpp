// SPDX-License-Identifier: GPL-3.0-or-later
#include "websearch.hpp"

#include "../../003_stylize/qwen14b.hpp"

#include <cctype>
#include <string>

namespace websearch {
namespace {

constexpr const char * kSystemPrompt =
    "You help a local developer retrieve a JavaScript / front-end library "
    "to vendor into a project. Given the user's request and a list of web "
    "search results, choose ONE well-known library that fits, and give "
    "CONCRETE direct-download URLs for a single self-contained build file "
    "(a standalone UMD / IIFE / 'production' bundle that works via a plain "
    "<script src> tag with no bundler).\n"
    "\n"
    "Rules:\n"
    "- Prefer jsDelivr (https://cdn.jsdelivr.net/npm/<pkg>[@ver]/<path>) or "
    "unpkg (https://unpkg.com/<pkg>[@ver]/<path>).\n"
    "- Point at the standalone/UMD/minified build, NOT an ESM module that "
    "needs importing.\n"
    "- Give 1 to 3 candidate URLs, best first; they should all fetch the "
    "same kind of standalone build in case one 404s.\n"
    "- Pick a sensible local filename ending in .js.\n"
    "\n"
    "Also state the library's CURRENT API for the core task in one line, "
    "using the exact global/method names a plain <script> user calls "
    "(this guards against the integrator assuming an outdated API).\n"
    "\n"
    "Respond with EXACTLY these lines and nothing else:\n"
    "PACKAGE: <library name>\n"
    "FILENAME: <name>.js\n"
    "URL1: <direct url>\n"
    "URL2: <direct url or ->\n"
    "URL3: <direct url or ->\n"
    "APIHINT: <one line: exact current API calls to use>\n"
    "\n"
    "EXAMPLE:\n"
    "PACKAGE: lightweight-charts\n"
    "FILENAME: lightweight-charts.standalone.js\n"
    "URL1: https://cdn.jsdelivr.net/npm/lightweight-charts/dist/lightweight-charts.standalone.production.js\n"
    "URL2: https://unpkg.com/lightweight-charts/dist/lightweight-charts.standalone.production.js\n"
    "URL3: -\n"
    "APIHINT: v5 global LightweightCharts; const chart = "
    "LightweightCharts.createChart(el, opts); const s = "
    "chart.addSeries(LightweightCharts.CandlestickSeries, opts); "
    "s.setData([{time,open,high,low,close}]). Do NOT use addCandlestickSeries "
    "(removed in v5).\n";

std::string trim(const std::string & s) {
    std::size_t b = 0, e = s.size();
    auto ws = [](char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; };
    while (b < e && ws(s[b])) ++b;
    while (e > b && ws(s[e - 1])) --e;
    return s.substr(b, e - b);
}

std::string lc(const std::string & s) {
    std::string o = s;
    for (auto & c : o) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return o;
}

std::string line_value(const std::string & body, const std::string & key) {
    const std::string lb = lc(body), lk = lc(key);
    std::size_t p = lb.find(lk);
    if (p == std::string::npos) return {};
    p += lk.size();
    std::size_t e = body.find('\n', p);
    if (e == std::string::npos) e = body.size();
    return trim(body.substr(p, e - p));
}

bool is_http_url(const std::string & u) {
    return u.rfind("http://", 0) == 0 || u.rfind("https://", 0) == 0;
}

}  // namespace

DownloadPlan plan_download(std::string_view request, const std::vector<Hit> & hits) {
    DownloadPlan plan;

    std::string user;
    user += "REQUEST:\n";
    user += std::string(request) + "\n\n";
    user += "SEARCH RESULTS:\n";
    user += format_hits(hits);

    std::string out;
    try {
        out = qwen14b::generate(kSystemPrompt, user, /*max_new_tokens=*/220);
    } catch (...) {
        plan.note = "model unavailable for download planning";
        return plan;
    }

    plan.package  = line_value(out, "PACKAGE:");
    plan.filename = line_value(out, "FILENAME:");
    plan.api_hint = line_value(out, "APIHINT:");
    if (plan.api_hint == "-") plan.api_hint.clear();
    for (const char * key : {"URL1:", "URL2:", "URL3:"}) {
        const std::string u = line_value(out, key);
        if (is_http_url(u)) plan.urls.push_back(u);
    }
    if (plan.filename.empty() || plan.filename.find(".js") == std::string::npos) {
        plan.filename = plan.package.empty() ? "library.js" : plan.package + ".js";
    }
    // Sanitize filename to a bare basename.
    {
        auto slash = plan.filename.find_last_of("/\\");
        if (slash != std::string::npos) plan.filename = plan.filename.substr(slash + 1);
        std::string clean;
        for (char c : plan.filename) {
            if (std::isalnum(static_cast<unsigned char>(c)) || c == '.' || c == '-' || c == '_')
                clean.push_back(c);
        }
        if (clean.empty() || clean == ".js") clean = "library.js";
        plan.filename = clean;
    }
    plan.ok = !plan.urls.empty();
    if (!plan.ok && plan.note.empty()) plan.note = "no candidate download URL produced";
    return plan;
}

}  // namespace websearch
