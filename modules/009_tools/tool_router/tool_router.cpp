// SPDX-License-Identifier: GPL-3.0-or-later
#include "tool_router.hpp"

#include "../planner/planner.hpp"
#include "../shell/coder.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cstdio>
#include <mutex>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace tool_router {

namespace {

using json = nlohmann::json;

// Registry state. Guarded by g_mtx so multiple init()/register_tool()
// calls from concurrent threads don't race.
std::mutex          g_mtx;
std::vector<Tool>   g_tools;
bool                g_defaults_installed = false;

// ---------------------------------------------------------------------
// Default tool registry.
//
// Every tool listed here is one the /api/chat pipeline can dispatch
// directly. Adding a new tool means adding one entry below plus a
// dispatch case in server.cpp; no new regex is required.
// ---------------------------------------------------------------------
void register_defaults_unlocked() {
    if (g_defaults_installed) return;
    g_defaults_installed = true;

    g_tools.push_back({
        "ticket_plan",
        "Bulk-decompose a project goal into an ordered set of "
        "implementation tickets that a coding assistant can execute "
        "one at a time. Pick this when the user asks to plan, "
        "decompose, or create tickets for a project.",
        R"({"goal": "<the user's project goal, verbatim>"})",
        "You are a project-plan decomposer. Given a project goal, "
        "produce a set of SHORT, SELF-CONTAINED tickets that a coding "
        "assistant can execute one at a time in the order given. Each "
        "ticket body IS a prompt: written in imperative voice, "
        "unambiguous, actionable. No cross-references to other "
        "tickets. No plan-level commentary.\n"
        "\n"
        "Ticket-shape rules:\n"
        "  (1) Every ticket body is a single self-contained prompt "
        "written in imperative voice.\n"
        "  (2) One deliverable per ticket. Never chain multiple "
        "artifacts into one ticket.\n"
        "  (3) For every image sprite/tile/icon the project needs, "
        "write ONE dedicated art ticket whose body reads exactly: "
        "'Draw <detailed art description of ONE image>. Save to "
        "assets/<name>.png.' Do NOT mention any tool name.\n"
        "  (4) Code tickets must reference visual assets by their "
        "file paths (assets/robot.png, assets/wall.png, etc.).\n"
        "  (5) Each code ticket's body must give the coder enough "
        "context to succeed in ISOLATION: what file to create or "
        "modify, what function or entrypoint to add, which headers "
        "to include, which existing header signatures to preserve.\n"
        "  (6) Do NOT invent HTTP libraries or frameworks unless the "
        "user specified one. For C++ web servers prefer cpp-httplib "
        "(single header, MIT).\n"
        "  (7) Do NOT wrap file contents in markdown code fences in "
        "the ticket body; describe what to write instead.\n"
        "  (8) Aim for 10-16 tickets total. More if the project "
        "genuinely needs it.\n"
        "\n"
        "OUTPUT REQUIREMENTS:\n"
        "  - Emit exactly one JSON array. No prose before or after. "
        "No markdown code fences (no triple backticks).\n"
        "  - Shape: [{\"title\": \"...\", \"body\": \"...\"}, ...]\n"
        "  - Every element MUST have both fields non-empty.\n"
        "  - Title <= 60 characters, body <= 400 characters.\n"
        "  - Never use an em dash (U+2014), en dash (U+2013), or "
        "horizontal bar (U+2015). Use a plain hyphen (U+002D) when a "
        "dash is needed. This is a hard operator policy.\n"
        "\n"
        "Goal:\n{user_prompt}",
    });

    g_tools.push_back({
        "ticket_create",
        "Create ONE single ticket on the project board. Pick this when "
        "the user asks to add a single ticket, not a plan or multiple.",
        R"({"title": "<short title>", "body": "<optional body>"})",
        "You are recording a single ticket. Title: {title}. Body: "
        "{body}. Confirm creation only. Never use an em dash (U+2014), "
        "en dash (U+2013), or horizontal bar (U+2015). Use a plain "
        "hyphen (U+002D) when a dash is needed. This is a hard "
        "operator policy.",
    });

    g_tools.push_back({
        "ticket_patch",
        "Edit one field (title, body, status, priority) on an existing "
        "ticket. Pick when the user names a T-N id and a change.",
        R"({"id": "T-<n>", "title?": "<new title>", "body?": "...", "status?": "todo|doing|blocked|done", "priority?": "low|normal|high|urgent"})",
        "Apply the requested patch to {id}. Change described: "
        "{user_prompt}",
    });

    g_tools.push_back({
        "ticket_move",
        "Move an existing ticket to a different column. Pick when the "
        "user says 'move T-N to done' / 'close T-N' / similar.",
        R"({"id": "T-<n>", "status": "todo|doing|blocked|done"})",
        "Move {id} to {status}.",
    });

    g_tools.push_back({
        "ticket_remove",
        "Delete an existing ticket from the board.",
        R"({"id": "T-<n>"})",
        "Delete ticket {id}.",
    });

    g_tools.push_back({
        "ticket_show",
        "Read one ticket by id and return its content.",
        R"({"id": "T-<n>"})",
        "Show ticket {id}.",
    });

    g_tools.push_back({
        "ticket_list",
        "Show the board (all tickets grouped by column). Pick when the "
        "user asks 'list tickets', 'show the board', etc. No args.",
        R"({})",
        "List every ticket, grouped by status column.",
    });

    g_tools.push_back({
        "image_gen",
        "Generate a new image from a text subject (Chroma1-HD via "
        "sd-cli). Pick when the user asks to draw, generate, produce, "
        "paint, or make a picture / sprite / tile / icon. Include a "
        "save-to path in args when the user names one. When the "
        "request refers to a recurring named character or a shared "
        "subject across a series ('the same robot', 'our maze robot', "
        "'walking robot' after an earlier 'idle robot'), also fill "
        "character_name with a short slug so the generator can apply "
        "canonical seed + reference + LoRA continuity.",
        R"({"subject": "<concrete visual description>", "save_to?": "<path relative to project root>", "character_name?": "<short slug identifying a recurring character>"})",
        "You are producing a concrete subject for a text-to-image "
        "generator. Rewrite the user's request as a visually specific "
        "subject: name the primary object, its shape, colors, style, "
        "view angle. Keep under 40 words. No prose about the "
        "generator itself, no mentions of tools. Output ONLY the "
        "subject description. Never use an em dash (U+2014), en dash "
        "(U+2013), or horizontal bar (U+2015). Use a plain hyphen "
        "(U+002D) when a dash is needed. This is a hard operator "
        "policy.\n"
        "\nUser request: {user_prompt}",
    });

    g_tools.push_back({
        "image_promote",
        "Lock an existing image in as the canonical reference for a "
        "named character. Pick when the operator says 'promote this', "
        "'lock this in as the canonical <name>', 'use this as our <name> "
        "reference', 'make this the canonical <name>', or similar. The "
        "source image can be named explicitly ('promote assets/robot.png "
        "as karth') or inherited from the most recent image_gen turn.",
        R"({"char_name": "<short character slug, e.g. 'robot' or 'karth'>", "source_image_path?": "<absolute or project-relative image path; may be omitted to use last generated image>"})",
        "Confirm the promotion of {source_image_path} as the canonical "
        "reference for character '{char_name}'. Every future image_gen "
        "call naming this character will use this reference for seed / "
        "img2img / LoRA continuity.",
    });

    g_tools.push_back({
        "train_canonical",
        "Kick off a LoRA training run against Chroma1-HD for a named "
        "character, using the approved images stashed under "
        ".ac9_images/canonical/<char>/. Pick when the operator says "
        "'train a LoRA for <name>', 'train canonical <name>', 'lock in "
        "<name> as a persistent identity token', or similar. Requires "
        "5-15 approved images already promoted.",
        R"({"char_name": "<short character slug>"})",
        "Begin LoRA training for character '{char_name}' against "
        "Chroma1-HD via ostris/ai-toolkit. Report progress and land "
        "the resulting .lora.safetensors under the canonical directory.",
    });

    g_tools.push_back({
        "image_edit",
        "Modify an existing image (Chroma img2img). Pick when the "
        "user asks to edit, modify, recolor, or restyle an image "
        "they already have.",
        R"({"edit_op": "<what to change>", "target_hint?": "<file or description of which image>"})",
        "Restate the requested edit as an imperative img2img "
        "instruction. Preserve the original subject; only describe "
        "the change. Output ONLY the imperative sentence. Never use "
        "an em dash (U+2014), en dash (U+2013), or horizontal bar "
        "(U+2015). Use a plain hyphen (U+002D) when a dash is needed. "
        "This is a hard operator policy.\n"
        "\nUser request: {user_prompt}",
    });

    g_tools.push_back({
        "mouser_search",
        "Look up an electronic part on Mouser. Pick when the user "
        "asks about stock, availability, or a part number.",
        R"({"keyword": "<mouser search query>"})",
        "You are producing a Mouser SearchByKeyword payload. Extract "
        "the manufacturer part number or key search phrase from the "
        "user's request and output ONLY that keyword string. Never "
        "use an em dash (U+2014), en dash (U+2013), or horizontal bar "
        "(U+2015). Use a plain hyphen (U+002D) when a dash is needed. "
        "This is a hard operator policy.\n"
        "\nUser request: {user_prompt}",
    });

    g_tools.push_back({
        "coder",
        "Write or modify source files, or run shell commands, in the "
        "current project. Pick this when the user's request is 'do "
        "this coding thing' with no more specific tool matching. This "
        "is the default for ticket-runner bodies that describe file "
        "writes, build steps, or verification commands.",
        R"({"task": "<the task description, verbatim>"})",
        "{user_prompt}",
    });

    // -- Office document tools (.odt / .ods / .odp / .odg / .docx /
    //    .xlsx / .pptx). Backed by office_docs::* which shells out to
    //    LibreOffice / Collabora soffice to convert to plain text /
    //    CSV / HTML and back. Every prompt_template repeats the
    //    em-dash ban so downstream summaries do not smuggle typography
    //    into artifacts.
    g_tools.push_back({
        "doc_read",
        "Extract the plain-text contents of an Office document "
        "(.odt / .ods / .odp / .odg / .docx / .xlsx / .pptx). Pick when "
        "the user asks to read, open, or show the contents of a "
        "document, spreadsheet, or presentation file.",
        R"({"path": "<absolute or project-relative path to the doc>"})",
        "Confirm you are extracting text from {path}. Report only the "
        "extracted body verbatim - no wrapping commentary. Never use "
        "an em dash (U+2014), en dash (U+2013), or horizontal bar "
        "(U+2015). Use a plain hyphen when a dash is needed.",
    });

    g_tools.push_back({
        "doc_summarize",
        "Read an Office document and produce a 3-sentence summary. "
        "Pick when the user asks for a summary, overview, or gist of "
        "a .odt / .docx / .ods / .xlsx / .odp / .pptx / .odg file.",
        R"({"path": "<absolute or project-relative path to the doc>"})",
        "You are producing a 3-sentence summary of the Office document "
        "at {path}. Output exactly three sentences, no bullet list, no "
        "preamble. Never use an em dash (U+2014), en dash (U+2013), or "
        "horizontal bar (U+2015). Use a plain hyphen when a dash is "
        "needed. This is a hard operator policy.",
    });

    g_tools.push_back({
        "doc_write",
        "Create a new Office document from plain text. Pick when the "
        "user says 'write a document', 'draft a .docx', 'make an odt "
        "with the following text', or names an output path ending in "
        ".odt / .docx / .txt.",
        R"({"path": "<destination path, extension picks the format>", "content": "<plain-text body>", "format?": "odt|docx|txt"})",
        "You are drafting the body of an Office document to be saved "
        "at {path}. Emit ONLY the plain-text body: no markdown fences, "
        "no HTML tags. Preserve paragraph breaks with blank lines. "
        "Never use an em dash (U+2014), en dash (U+2013), or "
        "horizontal bar (U+2015). Use a plain hyphen when a dash is "
        "needed. This is a hard operator policy.\n"
        "\nUser request: {user_prompt}",
    });

    g_tools.push_back({
        "doc_edit",
        "Apply find/replace text patches to an existing Office "
        "document. Pick when the user asks to change, rename, or fix "
        "wording inside a .odt / .docx / .ods / .xlsx / .odp / .pptx.",
        R"({"path": "<doc path>", "changes": [{"find": "<needle>", "replace": "<replacement>", "whole_word?": false}, ...]})",
        "Confirm the edit plan for {path}. List each substitution as "
        "'find -> replace' on its own line. Never use an em dash "
        "(U+2014), en dash (U+2013), or horizontal bar (U+2015). Use "
        "a plain hyphen when a dash is needed.",
    });

    g_tools.push_back({
        "doc_structure",
        "Describe the structural outline of an Office document: sheet "
        "names + row/col counts for a spreadsheet, slide count + "
        "titles for a deck, heading tree for a word-processor doc. "
        "Pick when the user asks 'what's in this file', 'list the "
        "sheets', 'show me the slide titles', or similar.",
        R"({"path": "<doc path>"})",
        "Report the structural outline for {path} as a compact "
        "human-readable list. Never use an em dash (U+2014), en dash "
        "(U+2013), or horizontal bar (U+2015). Use a plain hyphen "
        "when a dash is needed.",
    });

    g_tools.push_back({
        "sheet_read",
        "Extract a spreadsheet sheet as CSV (.ods / .xlsx). Pick when "
        "the user asks for the raw rows of a sheet, or names a "
        "specific tab.",
        R"({"path": "<spreadsheet path>", "sheet_name?": "<optional tab name>"})",
        "You are extracting sheet '{sheet_name}' from {path} as CSV. "
        "Output only the CSV rows. Never use an em dash (U+2014), en "
        "dash (U+2013), or horizontal bar (U+2015). Use a plain hyphen "
        "when a dash is needed.",
    });

    g_tools.push_back({
        "sheet_write",
        "Write a CSV body into an .ods or .xlsx file. Pick when the "
        "user asks to create or update a spreadsheet from tabular data.",
        R"({"path": "<spreadsheet path>", "csv": "<CSV body>", "sheet_name?": "<optional tab name>"})",
        "You are producing the CSV body destined for {path} sheet "
        "'{sheet_name}'. Emit only CSV rows separated by newlines, no "
        "prose. Never use an em dash (U+2014), en dash (U+2013), or "
        "horizontal bar (U+2015). Use a plain hyphen when a dash is "
        "needed.\n"
        "\nUser request: {user_prompt}",
    });

    g_tools.push_back({
        "slide_read",
        "Extract text from a presentation (.odp / .pptx) - either the "
        "full deck or one slide by 1-based index. Pick when the user "
        "asks to read, quote, or open a slide.",
        R"({"path": "<presentation path>", "slide_num?": 0})",
        "Confirm the slide-read plan for {path} (slide index "
        "{slide_num}). Output only the extracted slide text. Never use "
        "an em dash (U+2014), en dash (U+2013), or horizontal bar "
        "(U+2015). Use a plain hyphen when a dash is needed.",
    });

    g_tools.push_back({
        "slide_write",
        "Build a presentation deck (.odp / .pptx) from a list of "
        "{title, body} slides. Pick when the user asks to draft a "
        "slide deck from bullet points, an outline, or a topic.",
        R"({"path": "<destination deck path>", "slides": [{"title": "<slide title>", "body": "<bullet lines separated by newline>"}, ...]})",
        "You are drafting slides destined for {path}. Emit ONLY a "
        "JSON array of {\"title\":\"...\",\"body\":\"...\"} objects. "
        "Bodies are bullet lines separated by newline; no markdown "
        "list characters. Never use an em dash (U+2014), en dash "
        "(U+2013), or horizontal bar (U+2015). Use a plain hyphen "
        "when a dash is needed. This is a hard operator policy.\n"
        "\nUser request: {user_prompt}",
    });

    g_tools.push_back({
        "answerer",
        "Answer a general knowledge or reference question. Pick when "
        "the user is asking for information rather than an action.",
        R"({"question": "<the user's question, verbatim>"})",
        "{user_prompt}",
    });

    g_tools.push_back({
        "statement_note",
        "Record a persistent statement the user is telling the system "
        "to remember (a preference, a fact about the project). Pick "
        "when the user is teaching, not asking or commanding.",
        R"({"fact": "<the statement, verbatim>"})",
        "{user_prompt}",
    });
}

// Build the router system prompt from the current registry.
std::string build_router_system_prompt() {
    std::ostringstream ss;
    ss << "You are a strict tool router. Given a user request, pick "
          "the ONE registered tool that best matches, or say 'none' if "
          "no tool matches.\n\n"
          "AVAILABLE TOOLS:\n";
    for (const auto & t : g_tools) {
        ss << "  * " << t.name << " - " << t.description
           << "\n    args schema: " << t.args_schema << "\n";
    }
    ss << "  * none - no registered tool applies (the pipeline will "
          "fall back to its default handling).\n\n"
          "OUTPUT REQUIREMENTS:\n"
          "  - Emit EXACTLY one JSON object. No prose before or after. "
          "No markdown code fences. No <think> block.\n"
          "  - Shape:\n"
          "      {\n"
          "        \"tool\":       <one of the tool names above, or "
          "\"none\">,\n"
          "        \"args\":       { ... matching that tool's args "
          "schema, or {} },\n"
          "        \"confidence\": <number 0.0-1.0>,\n"
          "        \"reason\":     \"<short rationale>\"\n"
          "      }\n"
          "  - Use LOW confidence (< 0.6) when unsure. Use \"none\" and "
          "confidence 0 when nothing matches.\n"
          "  - Do NOT invent tool names. Do NOT return a list.\n"
          "  - Never use an em dash (U+2014), en dash (U+2013), or "
          "horizontal bar (U+2015). Use a plain hyphen (U+002D) when a "
          "dash is needed. This is a hard operator policy.";
    return ss.str();
}

// Extract the first `{...}` JSON object substring from LLM output.
// Handles well-formed responses plus stray prose or fenced blocks the
// router prompt tried to forbid but the model still emitted.
std::string extract_json_object(const std::string & s) {
    int         depth = 0;
    std::size_t start = std::string::npos;
    for (std::size_t i = 0; i < s.size(); ++i) {
        const char c = s[i];
        if (c == '{') {
            if (depth == 0) start = i;
            ++depth;
        } else if (c == '}') {
            --depth;
            if (depth == 0 && start != std::string::npos) {
                return s.substr(start, i - start + 1);
            }
        }
    }
    return {};
}

// Simple `{name}` -> value substitution used by shape(). Values come
// from Choice.args (any scalar or short string) plus a couple of
// well-known keys (user_prompt).
std::string substitute(const std::string & tmpl,
                        const std::unordered_map<std::string, std::string> & vars)
{
    std::string out;
    out.reserve(tmpl.size());
    for (std::size_t i = 0; i < tmpl.size();) {
        if (tmpl[i] == '{') {
            const std::size_t end = tmpl.find('}', i);
            if (end != std::string::npos) {
                const std::string key = tmpl.substr(i + 1, end - i - 1);
                auto it = vars.find(key);
                if (it != vars.end()) {
                    out.append(it->second);
                    i = end + 1;
                    continue;
                }
            }
        }
        out.push_back(tmpl[i]);
        ++i;
    }
    return out;
}

// Ask the LLM. Uses planner-4b when AC9_TOOL_ROUTER_ROLE=planner is
// set (fast, thinking-optimized), otherwise reuses the coder runtime
// (qwen35 - already warm for most calls). Returns the raw response.
std::string call_llm(const std::string & system_prompt,
                     const std::string & user_msg) {
    const char * role_env = std::getenv("AC9_TOOL_ROUTER_ROLE");
    const std::string role = role_env ? role_env : "";

    if (role == "planner") {
        try {
            return planner::generate(system_prompt, user_msg,
                                     /*max_new_tokens=*/512, nullptr);
        } catch (const std::exception & ex) {
            std::fprintf(stderr,
                "tool_router: planner threw: %s; falling back to coder\n",
                ex.what());
        }
    }
    try {
        return coder::generate(system_prompt, user_msg,
                               /*max_new_tokens=*/512, nullptr);
    } catch (const std::exception & ex) {
        // Loud stderr marker: this is the exact spot where a silent
        // CUDA OOM in coder::generate would otherwise disappear (the
        // caller sees an empty string, treats it as "no LLM decision",
        // and falls through to the regex routers — invisible failure).
        std::fprintf(stderr,
            "!!!! CODER MODEL FAILED !!!! tool_router::call_llm caught: %s\n"
            "!!!! Next art / regex-matched tickets may still pass, but "
            "code tickets requiring the LLM will produce empty output "
            "and eventually block the runner. Check GPU 0 / 1 VRAM.\n",
            ex.what());
        return {};
    }
}

}  // anonymous namespace

// ------ Public API ---------------------------------------------------

void init() {
    std::lock_guard<std::mutex> lk(g_mtx);
    register_defaults_unlocked();
}

void register_defaults() {
    std::lock_guard<std::mutex> lk(g_mtx);
    g_tools.clear();
    g_defaults_installed = false;
    register_defaults_unlocked();
}

void register_tool(Tool t) {
    std::lock_guard<std::mutex> lk(g_mtx);
    for (auto & existing : g_tools) {
        if (existing.name == t.name) { existing = std::move(t); return; }
    }
    g_tools.push_back(std::move(t));
}

const std::vector<Tool> & registry() {
    std::lock_guard<std::mutex> lk(g_mtx);
    return g_tools;
}

const Tool * find(std::string_view name) {
    std::lock_guard<std::mutex> lk(g_mtx);
    for (const auto & t : g_tools) {
        if (t.name == name) return &t;
    }
    return nullptr;
}

Choice route(const std::string & cleaned_prompt,
             const std::string & wiki_block,
             const std::string & dict_block) {
    Choice c;
    c.tool = "none";
    c.confidence = 0.0;

    // Ensure defaults are installed even if init() was never called.
    {
        std::lock_guard<std::mutex> lk(g_mtx);
        register_defaults_unlocked();
    }

    const std::string sys_prompt = build_router_system_prompt();
    std::ostringstream body;
    body << "USER REQUEST:\n" << cleaned_prompt << "\n";
    if (!wiki_block.empty())
        body << "\nRELEVANT WIKIPEDIA CONTEXT:\n" << wiki_block << "\n";
    if (!dict_block.empty())
        body << "\nRELEVANT DICTIONARY CONTEXT:\n" << dict_block << "\n";
    body << "\nOUTPUT the JSON object now.";

    const std::string raw = call_llm(sys_prompt, body.str());
    if (raw.empty()) { c.reason = "LLM call failed"; return c; }

    const std::string obj_txt = extract_json_object(raw);
    if (obj_txt.empty()) {
        c.reason = "no JSON object in router output";
        return c;
    }

    json j = json::parse(obj_txt, nullptr, false);
    if (!j.is_object()) {
        c.reason = "router output was not a JSON object";
        return c;
    }
    // Parse fields defensively.
    try {
        c.tool = j.value("tool", std::string{"none"});
        c.confidence = j.value("confidence", 0.0);
        c.reason = j.value("reason", std::string{});
        if (j.contains("args") && j["args"].is_object())
            c.args = j["args"];
        else
            c.args = json::object();
    } catch (...) {
        c.tool = "none";
        c.confidence = 0.0;
        c.reason = "router output missing required fields";
        return c;
    }
    if (c.confidence < 0.0) c.confidence = 0.0;
    if (c.confidence > 1.0) c.confidence = 1.0;

    // Guardrail: if the router names a tool that doesn't exist, treat
    // it as no match. The LLM occasionally hallucinates tool names not
    // in the registry ("shell_command", "search_web") no matter how
    // strict the prompt.
    if (c.tool != "none" && !find(c.tool)) {
        c.reason = std::string("router picked unknown tool: ") + c.tool +
                   " (" + c.reason + ")";
        c.tool = "none";
        c.confidence = 0.0;
    }
    return c;
}

void shape(Choice & c, const std::string & raw_user_prompt) {
    if (c.tool == "none") return;
    const Tool * t = find(c.tool);
    if (!t) return;

    // Build variable map from args + well-known keys.
    std::unordered_map<std::string, std::string> vars;
    vars["user_prompt"] = raw_user_prompt;
    if (c.args.is_object()) {
        for (auto it = c.args.begin(); it != c.args.end(); ++it) {
            const std::string & k = it.key();
            if (it.value().is_string())
                vars[k] = it.value().get<std::string>();
            else if (it.value().is_number_integer())
                vars[k] = std::to_string(it.value().get<long long>());
            else if (it.value().is_number_float())
                vars[k] = std::to_string(it.value().get<double>());
            else
                vars[k] = it.value().dump();
        }
    }
    c.shaped_system = t->prompt_template;   // Whole template goes into
                                             // the system prompt slot by
                                             // convention. Downstream may
                                             // split it if needed.
    c.shaped_system = substitute(c.shaped_system, vars);
    c.shaped_user   = raw_user_prompt;
}

Choice route_and_shape(const std::string & cleaned_prompt,
                       const std::string & raw_user_prompt,
                       double confidence_threshold,
                       const std::string & wiki_block,
                       const std::string & dict_block) {
    Choice c = route(cleaned_prompt, wiki_block, dict_block);
    if (c.tool != "none" && c.confidence >= confidence_threshold) {
        shape(c, raw_user_prompt);
    }
    return c;
}

}  // namespace tool_router
