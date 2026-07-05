// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "../345_geom/geom.hpp"

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

// KiCad-equivalent in-memory data model, without wxWidgets.
//
// Coordinates are integer nanometers (kicad-native). Angles are
// EDA_ANGLE (degrees, stored as double).
//
// Polymorphism is expressed with an ItemType enum + a shallow base
// class hierarchy; downcasting is done via `as<T>()` which checks type.
// Serialization uses `type_id()` to switch. This mirrors KiCad's own
// KICAD_T / EDA_ITEM idiom while avoiding wx dependencies.
namespace kicad_model {

using UUID = std::string;   // "aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee"

UUID make_uuid();

// -------------------- item classification -----------------------------

enum class ItemType : std::uint16_t {
    // Common / schematic
    SchSymbol,
    SchWire,
    SchBus,
    SchLabel,
    SchGlobalLabel,
    SchHierLabel,
    SchNetclassFlag,
    SchJunction,
    SchNoConnect,
    SchBusEntry,
    SchText,
    SchTextBox,
    SchShape,
    SchBitmap,
    SchSheet,
    SchSheetPin,
    SchPin,

    // PCB
    PcbFootprint,
    PcbPad,
    PcbTrack,
    PcbArc,
    PcbVia,
    PcbZone,
    PcbGrLine,
    PcbGrArc,
    PcbGrCircle,
    PcbGrPolygon,
    PcbGrText,
    PcbDimension
};

// -------------------- fields (a KiCad property) ----------------------

struct Field {
    std::string name;
    std::string value;
    geom::VECTOR2I  at{0, 0};      // in nm
    geom::EDA_ANGLE angle{0};
    bool            hide     = false;
    bool            bold     = false;
    bool            italic   = false;
    double          font_h_mm = 1.27;
    double          font_v_mm = 1.27;
    std::string     justify;       // "left","right","center","top","bottom"
    UUID            uuid;
};

// -------------------- base ------------------------------------------

struct Item {
    ItemType type;
    UUID     uuid;

    virtual ~Item() = default;

    template <typename T> T *       as()       { return type == T::kType ? static_cast<T*>(this) : nullptr; }
    template <typename T> const T * as() const { return type == T::kType ? static_cast<const T*>(this) : nullptr; }

protected:
    explicit Item(ItemType t) : type(t) {}
};

using ItemPtr = std::shared_ptr<Item>;

// -------------------- SCHEMATIC MODEL --------------------------------

struct SchPin {
    static constexpr ItemType kType = ItemType::SchPin;
    std::string name;          // "VCC", "OUT", "~"
    std::string number;        // "1", "A2"
    std::string electrical;    // "input","output","bidirectional",...
    std::string shape;         // "line","clock","inverted",...
    geom::VECTOR2I  at{0,0};      // relative to symbol origin
    geom::EDA_ANGLE angle{0};     // 0/90/180/270 in the source
    double         length_mm = 2.54;
    UUID           uuid;
};

struct LibSymbol {
    // Library-side definition (lives inside .kicad_sym or the
    // (lib_symbols ...) block of a .kicad_sch).
    std::string          lib_id;         // "Timer:NE555"
    bool                 power           = false;
    bool                 exclude_from_sim = false;
    bool                 in_bom          = true;
    bool                 on_board        = true;
    std::string          extends;        // parent lib_id when using inheritance
    std::vector<Field>   fields;
    std::vector<SchPin>  pins;
    // Graphic children (rectangles, polylines, texts) are held as
    // opaque s-expression trees so we can round-trip without needing
    // full sub-parsers for every graphic primitive shape yet.
    std::vector<std::string> raw_graphics_sexpr;
    // Alternative units (unit > 1) are similarly held as raw text for now.
    std::vector<std::string> raw_unit_sexpr;
};

struct SchSymbol : Item {
    static constexpr ItemType kType = ItemType::SchSymbol;
    SchSymbol() : Item(kType) {}
    std::string     lib_id;        // "Timer:NE555"
    geom::VECTOR2I  at{0, 0};
    geom::EDA_ANGLE angle{0};
    int             unit = 1;
    bool            mirror_x = false;
    bool            mirror_y = false;
    bool            dnp      = false;
    bool            in_bom   = true;
    bool            on_board = true;
    std::vector<Field> fields;   // Reference, Value, Footprint, Datasheet, custom
    // Pin overrides (rare).
    std::unordered_map<std::string, UUID> pin_uuid_by_number;

    std::string reference() const { return field_value("Reference"); }
    std::string value()     const { return field_value("Value"); }
    std::string field_value(std::string_view name) const;
};

struct SchWire : Item {
    static constexpr ItemType kType = ItemType::SchWire;
    SchWire() : Item(kType) {}
    std::vector<geom::VECTOR2I> pts;  // usually 2; can be a polyline
    double stroke_mm = 0.0;           // 0 = default
    std::string stroke_type = "default";
    std::string stroke_color;
};
struct SchBus : Item {
    static constexpr ItemType kType = ItemType::SchBus;
    SchBus() : Item(kType) {}
    std::vector<geom::VECTOR2I> pts;
    double stroke_mm = 0.0;
    std::string stroke_type = "default";
    std::string stroke_color;
};

struct SchJunction : Item {
    static constexpr ItemType kType = ItemType::SchJunction;
    SchJunction() : Item(kType) {}
    geom::VECTOR2I at{0,0};
    double         diameter_mm = 0.0;   // 0 = default
    std::string    color;
};

struct SchNoConnect : Item {
    static constexpr ItemType kType = ItemType::SchNoConnect;
    SchNoConnect() : Item(kType) {}
    geom::VECTOR2I at{0,0};
};

struct SchBusEntry : Item {
    static constexpr ItemType kType = ItemType::SchBusEntry;
    SchBusEntry() : Item(kType) {}
    geom::VECTOR2I at{0,0};
    geom::VECTOR2I size{2540000, 2540000}; // 2.54mm x 2.54mm
    double stroke_mm = 0.0;
    std::string stroke_type = "default";
};

struct SchLabel : Item {
    static constexpr ItemType kType = ItemType::SchLabel;
    SchLabel() : Item(kType) {}
    std::string     text;
    geom::VECTOR2I  at{0,0};
    geom::EDA_ANGLE angle{0};
    std::string     shape;    // "input","output","bidirectional","tri_state","passive" for hier
    // Field siblings: intersheets_refs list etc.
    std::vector<Field> fields;
};
struct SchGlobalLabel : SchLabel {
    static constexpr ItemType kType = ItemType::SchGlobalLabel;
    SchGlobalLabel() { type = kType; }
};
struct SchHierLabel : SchLabel {
    static constexpr ItemType kType = ItemType::SchHierLabel;
    SchHierLabel() { type = kType; }
};

struct SchText : Item {
    static constexpr ItemType kType = ItemType::SchText;
    SchText() : Item(kType) {}
    std::string     text;
    geom::VECTOR2I  at{0,0};
    geom::EDA_ANGLE angle{0};
    double          font_h_mm = 1.27;
    double          font_v_mm = 1.27;
    bool            bold      = false;
    bool            italic    = false;
    std::string     justify;
};

struct SchTextBox : Item {
    static constexpr ItemType kType = ItemType::SchTextBox;
    SchTextBox() : Item(kType) {}
    std::string     text;
    geom::VECTOR2I  at{0,0};
    geom::VECTOR2I  size{0,0};
    geom::EDA_ANGLE angle{0};
    std::string     stroke_color;
    double          stroke_mm = 0.0;
    std::string     stroke_type = "default";
    std::string     fill_type   = "none";
    std::string     fill_color;
    double          font_h_mm = 1.27;
    double          font_v_mm = 1.27;
};

struct SchShape : Item {
    // (polyline (pts (xy ...) ...) (stroke ...) (fill ...))
    // (rectangle (start x y) (end x y) (stroke) (fill))
    // (circle (center x y) (radius ...) (stroke) (fill))
    // (arc (start x y) (mid x y) (end x y) (stroke) (fill))
    static constexpr ItemType kType = ItemType::SchShape;
    SchShape() : Item(kType) {}
    std::string   shape;            // "polyline" | "rectangle" | "circle" | "arc" | "bezier"
    std::vector<geom::VECTOR2I> pts; // polyline / bezier control points
    geom::VECTOR2I start{0,0};
    geom::VECTOR2I mid{0,0};
    geom::VECTOR2I end{0,0};
    geom::VECTOR2I center{0,0};
    long long      radius_nm = 0;
    double         stroke_mm = 0.0;
    std::string    stroke_type = "default";
    std::string    stroke_color;
    std::string    fill_type = "none";
    std::string    fill_color;
};

struct SchSheetPin {
    std::string     name;
    geom::VECTOR2I  at{0,0};
    geom::EDA_ANGLE angle{0};
    std::string     shape;   // "input","output","bidirectional",...
    UUID            uuid;
};

struct SchSheet : Item {
    static constexpr ItemType kType = ItemType::SchSheet;
    SchSheet() : Item(kType) {}
    geom::VECTOR2I  at{0,0};
    geom::VECTOR2I  size{25400000, 25400000};   // 25.4x25.4mm default
    std::string     name;
    std::string     file_name;
    std::vector<Field> fields;
    std::vector<SchSheetPin> pins;
    std::string     stroke_color;
    double          stroke_mm = 0.0;
    std::string     fill_type = "none";
};

struct SchScreen {
    // A single sheet's drawable page.
    std::string        paper = "A4";
    std::string        title;
    std::string        rev;
    std::string        comment[9];   // KiCad supports up to 9 comment lines
    std::vector<ItemPtr> items;
};

struct Schematic {
    // A schematic project. Top-level file is a root SchScreen; each
    // SchSheet references another SchScreen loaded from that file.
    std::string version;              // "20250114" etc.
    std::string generator = "ac9";
    std::string generator_version = "0.1";
    UUID        uuid;
    std::string paper = "A4";
    std::unordered_map<std::string, LibSymbol> lib_symbols;  // keyed by lib_id
    SchScreen   root;
    // Nested screens for hierarchical sheets, keyed by the sheet's UUID.
    std::unordered_map<UUID, SchScreen> child_screens;
};

// -------------------- PCB MODEL --------------------------------------

// One physical PCB layer. Canonical KiCad names: "F.Cu", "B.Cu",
// "F.SilkS", "Edge.Cuts", "In1.Cu", ...
struct LayerInfo {
    int         id       = 0;      // KiCad numeric layer id
    std::string canonical_name;    // "F.Cu"
    std::string user_name;         // display; usually the same
    std::string type;              // "signal" | "power" | "mixed" | "jumper" | "user"
};

struct Pad {
    static constexpr ItemType kType = ItemType::PcbPad;
    std::string     number;                     // "1", "A5"
    std::string     kind;                       // "smd","thru_hole","np_thru_hole","connect"
    std::string     shape;                      // "circle","rect","roundrect","oval","custom","trapezoid"
    geom::VECTOR2I  at{0,0};                    // relative to footprint origin
    geom::EDA_ANGLE angle{0};
    geom::VECTOR2I  size{0,0};                  // width x height in nm
    long long       drill_nm     = 0;           // 0 for smd
    long long       drill_slot_nm = 0;          // long-hole extent
    std::vector<std::string> layers;            // {"F.Cu","F.Mask","F.Paste"}
    double          roundrect_ratio = 0.0;
    int             net             = 0;
    std::string     net_name;
    double          solder_mask_margin_mm = 0.0;
    double          solder_paste_margin_mm = 0.0;
    UUID            uuid;
};

struct Footprint : Item {
    static constexpr ItemType kType = ItemType::PcbFootprint;
    Footprint() : Item(kType) {}
    std::string     lib_id;                  // "Package_DIP:DIP-8_W7.62mm"
    std::string     placement_layer = "F.Cu";
    geom::VECTOR2I  at{0,0};
    geom::EDA_ANGLE angle{0};
    std::string     attr;                    // "smd","through_hole","exclude_from_pos_files",...
    std::vector<Field> fields;               // Reference / Value / Footprint / ...
    std::vector<Pad>   pads;
    // Silk / fab / courtyard graphics kept as raw sexpr blocks for
    // round-trip fidelity in this pass; interactive editing will
    // eventually promote these to first-class SchShape-alikes.
    std::vector<std::string> raw_graphics_sexpr;
    std::string  descr;
    std::string  tags;
    UUID         uuid;
};

struct PcbTrack : Item {
    static constexpr ItemType kType = ItemType::PcbTrack;
    PcbTrack() : Item(kType) {}
    geom::VECTOR2I start{0,0};
    geom::VECTOR2I end{0,0};
    long long      width_nm = 0;
    std::string    layer = "F.Cu";
    int            net = 0;
    bool           locked = false;
};

struct PcbArc : Item {
    static constexpr ItemType kType = ItemType::PcbArc;
    PcbArc() : Item(kType) {}
    geom::VECTOR2I start{0,0};
    geom::VECTOR2I mid{0,0};
    geom::VECTOR2I end{0,0};
    long long      width_nm = 0;
    std::string    layer = "F.Cu";
    int            net = 0;
};

struct PcbVia : Item {
    static constexpr ItemType kType = ItemType::PcbVia;
    PcbVia() : Item(kType) {}
    std::string    via_type = "through";      // "through","blind","micro"
    geom::VECTOR2I at{0,0};
    long long      size_nm  = 0;
    long long      drill_nm = 0;
    std::vector<std::string> layers = {"F.Cu","B.Cu"};
    int            net = 0;
    bool           locked = false;
    bool           free   = false;
};

struct Zone : Item {
    static constexpr ItemType kType = ItemType::PcbZone;
    Zone() : Item(kType) {}
    int             net = 0;
    std::string     net_name;
    std::vector<std::string> layers = {"F.Cu"};
    long long       hatch_thickness_nm = 0;
    long long       hatch_gap_nm       = 0;
    long long       hatch_orientation_deg = 0;
    std::string     fill_mode = "solid";      // "solid","hatched"
    long long       clearance_nm = 0;
    long long       min_thickness_nm = 0;
    // Outline as sequence of polylines. In practice one outer chain +
    // any explicit keep-out sub-polygons.
    std::vector<geom::SHAPE_LINE_CHAIN> polys;
    // Fill polygons per layer (computed by zone fill).
    std::vector<geom::SHAPE_POLY_SET>   filled_polys;
    UUID            uuid;
};

struct GrLine : Item {
    static constexpr ItemType kType = ItemType::PcbGrLine;
    GrLine() : Item(kType) {}
    geom::VECTOR2I start{0,0};
    geom::VECTOR2I end{0,0};
    long long      width_nm = 0;
    std::string    layer = "Edge.Cuts";
    std::string    stroke_type = "default";
};

struct GrArc : Item {
    static constexpr ItemType kType = ItemType::PcbGrArc;
    GrArc() : Item(kType) {}
    geom::VECTOR2I start{0,0};
    geom::VECTOR2I mid{0,0};
    geom::VECTOR2I end{0,0};
    long long      width_nm = 0;
    std::string    layer = "Edge.Cuts";
};

struct GrCircle : Item {
    static constexpr ItemType kType = ItemType::PcbGrCircle;
    GrCircle() : Item(kType) {}
    geom::VECTOR2I center{0,0};
    geom::VECTOR2I mid{0,0};
    long long      width_nm = 0;
    std::string    layer = "Edge.Cuts";
    std::string    fill_type = "none";
};

struct GrPolygon : Item {
    static constexpr ItemType kType = ItemType::PcbGrPolygon;
    GrPolygon() : Item(kType) {}
    geom::SHAPE_LINE_CHAIN outline;
    long long              width_nm = 0;
    std::string            layer = "F.SilkS";
    std::string            fill_type = "solid";
};

struct GrText : Item {
    static constexpr ItemType kType = ItemType::PcbGrText;
    GrText() : Item(kType) {}
    std::string     text;
    geom::VECTOR2I  at{0,0};
    geom::EDA_ANGLE angle{0};
    std::string     layer = "F.SilkS";
    double          font_h_mm = 1.5;
    double          font_v_mm = 1.5;
    double          thickness_mm = 0.15;
    bool            bold = false;
    bool            italic = false;
    bool            mirror = false;
    std::string     justify;
};

struct NetInfo {
    int         id = 0;
    std::string name;         // "" for the unconnected net (id 0)
    // netclass name is resolved at project level; kept for convenience.
    std::string netclass = "Default";
};

struct Board {
    std::string version;                     // "20241229"
    std::string generator = "ac9";
    std::string generator_version = "0.1";
    UUID        uuid;
    std::string paper = "A4";
    double      thickness_mm = 1.6;
    bool        legacy_teardrops = false;
    std::vector<LayerInfo> layers;
    std::vector<NetInfo>   nets;              // nets[0] is the "" (id 0) net
    std::vector<ItemPtr>   items;             // footprints, tracks, arcs, vias, zones, gr_*
    // Raw setup s-expression preserved verbatim on read/write until
    // task 59 (board setup dialog) gives us first-class fields.
    std::string raw_setup_sexpr;
    // Raw plot params, embedded fonts marker etc.
    std::vector<std::string> raw_tail_sexpr;
};

// -------------------- Helpers ---------------------------------------

// Return the net id for a name; adds it if not present.
int intern_net(Board & board, std::string_view name);

// Standard 2-layer stackup, matches what our writer emits.
std::vector<LayerInfo> default_2layer_stackup();

} // namespace kicad_model
