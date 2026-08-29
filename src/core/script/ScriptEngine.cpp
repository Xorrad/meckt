#include "ScriptEngine.hpp"

#include "mod/Mod.hpp"
#include "provinces/ProvinceManager.hpp"
#include "titles/TitleManager.hpp"
#include "cultures/CultureManager.hpp"
#include "cultures/Culture.hpp"
#include "religions/ReligionManager.hpp"
#include "religions/Faith.hpp"
#include "regions/RegionManager.hpp"
#include "regions/Region.hpp"

namespace Script {

namespace {

/**
 * @brief Packs a range of pointers into a fresh 1-based Lua array, the shape
 *        every GetAll()-style binding returns.
 */
template <typename Range, typename Transform>
sol::table ToArray(sol::state& state, const Range& range, Transform transform) {
    sol::table array = state.create_table();
    int index = 1;
    for (const auto& element : range)
        array[index++] = transform(element);
    return array;
}

/**
 * @brief Casts a title to a tier-specific subclass, raising a Lua error naming
 *        the offending title when it isn't of that tier.
 * @note  Lua sees a single `Title` type, so tier-specific functions
 *        (dejure titles, barony province ids...) validate the tier here.
 */
template <typename T>
T* CastTitle(Title* title, const char* expected) {
    if (title == nullptr)
        throw std::runtime_error("title is nil");

    T* casted = dynamic_cast<T*>(title);
    if (casted == nullptr)
        throw std::runtime_error(fmt::format("title '{}' is not {}", title->GetName(), expected));

    return casted;
}

}

Engine::Engine(Mod& mod) :
    m_Mod(mod),
    m_State(),
    m_Docs(),
    m_Output({}),
    m_InstructionLimit(DefaultInstructionLimit),
    m_MapInvalidated(false)
{
    // Deliberately no `io`/`os`/`package`/`debug`: scripts edit mod data through
    // the bindings below, they have no business touching the filesystem or
    // loading native modules.
    m_State.open_libraries(
        sol::lib::base,
        sol::lib::string,
        sol::lib::table,
        sol::lib::math,
        sol::lib::utf8
    );

    this->BindUtility();
    this->BindTypes();
    this->BindManagers();
    this->BindMap();
}

//////////////////////////////////////////////////////

Result Engine::Run(const std::string& source) {
    m_Output.clear();
    m_MapInvalidated = false;

    lua_State* state = m_State.lua_state();

    // Abort runaway scripts rather than freezing the app with no way out. The
    // hook is (re)installed per run so a changed limit takes effect immediately.
    if (m_InstructionLimit > 0) {
        lua_sethook(state, [](lua_State* L, lua_Debug*) {
            luaL_error(L, "script aborted: instruction limit reached (infinite loop?)");
        }, LUA_MASKCOUNT, m_InstructionLimit);
    }
    else {
        lua_sethook(state, nullptr, 0, 0);
    }

    Result result;

    // Bindings report misuse by throwing, which sol2 turns into a Lua error;
    // the catch below is the backstop for anything thrown outside of that.
    try {
        sol::protected_function_result scriptResult = m_State.safe_script(source, sol::script_pass_on_error);

        result.success = scriptResult.valid();
        if (!result.success) {
            sol::error error = scriptResult;
            result.error = error.what();
        }
    }
    catch (const std::exception& e) {
        result.success = false;
        result.error = e.what();
    }

    lua_sethook(state, nullptr, 0, 0);

    result.output = m_Output;
    return result;
}

sol::state& Engine::GetState() {
    return m_State;
}

Docs& Engine::GetDocs() {
    return m_Docs;
}

const Docs& Engine::GetDocs() const {
    return m_Docs;
}

void Engine::SetInstructionLimit(int limit) {
    m_InstructionLimit = std::max(0, limit);
}

int Engine::GetInstructionLimit() const {
    return m_InstructionLimit;
}

bool Engine::IsMapInvalidated() const {
    return m_MapInvalidated;
}

void Engine::InvalidateMap() {
    m_MapInvalidated = true;
}

void Engine::Print(const std::string& text) {
    m_Output.push_back(text);
}

//////////////////////////////////////////////////////

void Engine::BindUtility() {
    auto global = MakeBinder(m_Docs, m_State.globals(), "globals");

    // Replaces the stock print(), which would otherwise write to a stdout the
    // script executor never shows.
    SCRIPT_FN(global, "print",
        "print(...)",
        "Writes its arguments to the output console, separated by tabs.",
        [this](sol::variadic_args args) {
            sol::protected_function tostring = m_State["tostring"];

            std::string line;
            for (auto arg : args) {
                if (!line.empty())
                    line += "\t";

                sol::protected_function_result text = tostring(arg);
                line += text.valid() ? text.get<std::string>() : "?";
            }
            this->Print(line);
        }
    );

    SCRIPT_FN(global, "log",
        "log(message)",
        "Writes a message to both the output console and the application log.",
        [this](const std::string& message) {
            LOG_INFO("[script] {}", message);
            this->Print(message);
        }
    );
}

void Engine::BindTypes() {
    //////////////////////////////////////////////////////
    // Enums
    //////////////////////////////////////////////////////

    m_State.new_enum<ProvinceFlags>("ProvinceFlags", {
        { "NONE",       ProvinceFlags::NONE },
        { "COASTAL",    ProvinceFlags::COASTAL },
        { "LAKE",       ProvinceFlags::LAKE },
        { "ISLAND",     ProvinceFlags::ISLAND },
        { "LAND",       ProvinceFlags::LAND },
        { "SEA",        ProvinceFlags::SEA },
        { "RIVER",      ProvinceFlags::RIVER },
        { "IMPASSABLE", ProvinceFlags::IMPASSABLE },
    });

    m_State.new_enum<TitleType>("TitleType", {
        { "BARONY",   TitleType::BARONY },
        { "COUNTY",   TitleType::COUNTY },
        { "DUCHY",    TitleType::DUCHY },
        { "KINGDOM",  TitleType::KINGDOM },
        { "EMPIRE",   TitleType::EMPIRE },
        { "HEGEMONY", TitleType::HEGEMONY },
    });

    m_State.new_enum<ClimateType>("ClimateType", {
        { "NONE",          ClimateType::NONE },
        { "MILD_WINTER",   ClimateType::MILD_WINTER },
        { "NORMAL_WINTER", ClimateType::NORMAL_WINTER },
        { "SEVERE_WINTER", ClimateType::SEVERE_WINTER },
    });

    //////////////////////////////////////////////////////
    // Color
    //////////////////////////////////////////////////////

    auto colorType = m_State.new_usertype<sf::Color>("Color",
        sol::constructors<sf::Color(), sf::Color(uint8_t, uint8_t, uint8_t), sf::Color(uint8_t, uint8_t, uint8_t, uint8_t)>(),
        "r", &sf::Color::r,
        "g", &sf::Color::g,
        "b", &sf::Color::b,
        "a", &sf::Color::a
    );
    auto color = MakeBinder(m_Docs, colorType, "Color");

    m_Docs.Register("Color", "r", "color.r / color.g / color.b / color.a", "The color's channels, each in [0, 255]. Readable and writable.");

    SCRIPT_FN(color, "ToHex",
        "color:ToHex() -> integer",
        "Returns the color packed as a 0xRRGGBBAA integer.",
        [](const sf::Color& self) { return self.toInteger(); }
    );

    SCRIPT_FN(color, "ToString",
        "color:ToString() -> string",
        "Returns the color as a readable \"(r, g, b, a)\" string.",
        [](const sf::Color& self) {
            return fmt::format("({}, {}, {}, {})", self.r, self.g, self.b, self.a);
        }
    );

    //////////////////////////////////////////////////////
    // Province
    //////////////////////////////////////////////////////

    auto provinceType = m_State.new_usertype<Province>("Province", sol::no_constructor);
    auto province = MakeBinder(m_Docs, provinceType, "Province");

    SCRIPT_FN(province, "GetId", "province:GetId() -> integer",
        "Returns the province's id.",
        [](Province& self) { return self.GetId(); });

    SCRIPT_FN(province, "SetId", "province:SetId(id)",
        "Sets the province's id. Prefer provinces.RenameId(), which keeps the manager's lookups in sync.",
        [](Province& self, int id) { self.SetId(id); });

    SCRIPT_FN(province, "GetName", "province:GetName() -> string",
        "Returns the province's name.",
        [](Province& self) { return self.GetName(); });

    SCRIPT_FN(province, "SetName", "province:SetName(name)",
        "Sets the province's name.",
        [](Province& self, const std::string& name) { self.SetName(name); });

    SCRIPT_FN(province, "GetColor", "province:GetColor() -> Color",
        "Returns the province's color on the provinces map.",
        [](Province& self) { return self.GetColor(); });

    SCRIPT_FN(province, "GetColorId", "province:GetColorId() -> integer",
        "Returns the province's color packed as an integer, the key it is stored under.",
        [](Province& self) { return self.GetColorId(); });

    SCRIPT_FN(province, "HasFlag", "province:HasFlag(flag) -> boolean",
        "Checks whether the province has a ProvinceFlags flag (e.g. ProvinceFlags.LAND).",
        [](Province& self, ProvinceFlags flag) { return self.HasFlag(flag); });

    SCRIPT_FN(province, "SetFlag", "province:SetFlag(flag, enabled)",
        "Adds or removes a ProvinceFlags flag on the province.",
        [this](Province& self, ProvinceFlags flag, bool enabled) {
            self.SetFlag(flag, enabled);
            this->InvalidateMap();
        });

    SCRIPT_FN(province, "GetFlagsString", "province:GetFlagsString() -> string",
        "Returns the province's flags as a readable comma-separated string.",
        [](Province& self) { return ProvinceFlagsToString(self.GetFlags()); });

    SCRIPT_FN(province, "GetHolding", "province:GetHolding() -> string",
        "Returns the province's holding type.",
        [](Province& self) { return self.GetHolding(); });

    SCRIPT_FN(province, "SetHolding", "province:SetHolding(holding)",
        "Sets the province's holding type (see provinces.GetHoldingTypes()).",
        [](Province& self, const std::string& holding) { self.SetHolding(holding); });

    SCRIPT_FN(province, "GetTerrain", "province:GetTerrain() -> string",
        "Returns the province's terrain type.",
        [](Province& self) { return self.GetTerrain(); });

    SCRIPT_FN(province, "SetTerrain", "province:SetTerrain(terrain)",
        "Sets the province's terrain type (see provinces.GetTerrainTypes()).",
        [this](Province& self, const std::string& terrain) {
            self.SetTerrain(terrain);
            this->InvalidateMap();
        });

    SCRIPT_FN(province, "GetCulture", "province:GetCulture() -> string",
        "Returns the name of the province's culture.",
        [](Province& self) { return self.GetCulture(); });

    SCRIPT_FN(province, "SetCulture", "province:SetCulture(culture)",
        "Sets the province's culture by name.",
        [this](Province& self, const std::string& culture) {
            self.SetCulture(culture);
            this->InvalidateMap();
        });

    SCRIPT_FN(province, "GetFaith", "province:GetFaith() -> string",
        "Returns the name of the province's faith.",
        [](Province& self) { return self.GetFaith(); });

    SCRIPT_FN(province, "SetFaith", "province:SetFaith(faith)",
        "Sets the province's faith by name.",
        [this](Province& self, const std::string& faith) {
            self.SetFaith(faith);
            this->InvalidateMap();
        });

    SCRIPT_FN(province, "GetClimate", "province:GetClimate() -> ClimateType",
        "Returns the province's climate type.",
        [](Province& self) { return self.GetClimateType(); });

    SCRIPT_FN(province, "SetClimate", "province:SetClimate(climate)",
        "Sets the province's climate type (e.g. ClimateType.SEVERE_WINTER).",
        [this](Province& self, ClimateType climate) {
            self.SetClimateType(climate);
            this->InvalidateMap();
        });

    SCRIPT_FN(province, "GetWinterSeverityBias", "province:GetWinterSeverityBias() -> string",
        "Returns the province's winter severity bias, as written in the mod files.",
        [](Province& self) { return self.GetWinterSeverityBias(); });

    SCRIPT_FN(province, "SetWinterSeverityBias", "province:SetWinterSeverityBias(bias)",
        "Sets the province's winter severity bias (a string, e.g. \"0.5\").",
        [this](Province& self, const std::string& bias) {
            self.SetWinterSeverityBias(bias);
            this->InvalidateMap();
        });

    SCRIPT_FN(province, "GetPixelCount", "province:GetPixelCount() -> integer",
        "Returns how many pixels the province covers on the provinces map.",
        [](Province& self) { return self.GetImagePixelsCount(); });

    SCRIPT_FN(province, "GetPosition", "province:GetPosition() -> x, y",
        "Returns the province's position on the provinces map, as two values.",
        [](Province& self) {
            sf::Vector2i position = self.GetImagePosition();
            return std::make_tuple(position.x, position.y);
        });

    SCRIPT_FN(province, "GetBarony", "province:GetBarony() -> Title|nil",
        "Returns the barony title holding this province, or nil if it has none.",
        [this](Province& self) -> Title* {
            return m_Mod.GetTitleManager().GetBaronyByProvinceId(self.GetId());
        });

    SCRIPT_FN(province, "GetLiegeTitle", "province:GetLiegeTitle(type) -> Title|nil",
        "Returns the province's governing title at a tier (e.g. TitleType.COUNTY), or nil.",
        [this](Province& self, TitleType type) {
            return self.GetProvinceLiegeTitle(m_Mod.GetTitleManager(), type);
        });

    //////////////////////////////////////////////////////
    // Title
    //////////////////////////////////////////////////////

    // Lua sees one `Title` type rather than the C++ hierarchy; the functions
    // that only make sense for one tier raise an error on the others.
    auto titleType = m_State.new_usertype<Title>("Title", sol::no_constructor);
    auto title = MakeBinder(m_Docs, titleType, "Title");

    SCRIPT_FN(title, "GetName", "title:GetName() -> string",
        "Returns the title's name, prefix included (e.g. \"c_paris\").",
        [](Title& self) { return self.GetName(); });

    SCRIPT_FN(title, "GetType", "title:GetType() -> TitleType",
        "Returns the title's tier.",
        [](Title& self) { return self.GetType(); });

    SCRIPT_FN(title, "GetTypeName", "title:GetTypeName() -> string",
        "Returns the title's tier as a readable string (e.g. \"County\").",
        [](Title& self) { return std::string(TitleTypeLabels[static_cast<int>(self.GetType())]); });

    SCRIPT_FN(title, "GetColor", "title:GetColor() -> Color",
        "Returns the title's color.",
        [](Title& self) { return self.GetColor(); });

    SCRIPT_FN(title, "SetColor", "title:SetColor(color)",
        "Sets the title's color.",
        [this](Title& self, const sf::Color& color) {
            self.SetColor(color);
            this->InvalidateMap();
        });

    SCRIPT_FN(title, "IsLandless", "title:IsLandless() -> boolean",
        "Checks whether the title is landless.",
        [](Title& self) { return self.IsLandless(); });

    SCRIPT_FN(title, "SetLandless", "title:SetLandless(landless)",
        "Sets whether the title is landless.",
        [](Title& self, bool landless) { self.SetLandless(landless); });

    SCRIPT_FN(title, "GetLiege", "title:GetLiege() -> Title|nil",
        "Returns the title's dejure liege, or nil if it has none.",
        [](Title& self) -> Title* { return self.GetLiegeTitle(); });

    SCRIPT_FN(title, "GetDejureTitles", "title:GetDejureTitles() -> Title[]",
        "Returns the titles held dejure by this title. Errors on baronies, which hold none.",
        [this](Title& self) {
            HighTitle* high = CastTitle<HighTitle>(&self, "a high title");
            return ToArray(m_State, high->GetDejureTitles(), [](Title* t) { return t; });
        });

    SCRIPT_FN(title, "AddDejureTitle", "title:AddDejureTitle(other)",
        "Adds a title to this title's dejure titles. Errors on baronies.",
        [this](Title& self, Title* other) {
            CastTitle<HighTitle>(&self, "a high title")->AddDejureTitle(other);
            this->InvalidateMap();
        });

    SCRIPT_FN(title, "RemoveDejureTitle", "title:RemoveDejureTitle(other)",
        "Removes a title from this title's dejure titles. Errors on baronies.",
        [this](Title& self, Title* other) {
            CastTitle<HighTitle>(&self, "a high title")->RemoveDejureTitle(other);
            this->InvalidateMap();
        });

    SCRIPT_FN(title, "GetCapital", "title:GetCapital() -> Title|nil",
        "Returns the title's capital county, or nil. Errors on baronies.",
        [](Title& self) -> Title* {
            return CastTitle<HighTitle>(&self, "a high title")->GetCapitalTitle();
        });

    SCRIPT_FN(title, "GetProvinceId", "title:GetProvinceId() -> integer",
        "Returns the province id of a barony title. Errors on any other tier.",
        [](Title& self) {
            return CastTitle<BaronyTitle>(&self, "a barony")->GetProvinceId();
        });

    SCRIPT_FN(title, "SetProvinceId", "title:SetProvinceId(id)",
        "Sets the province a barony title covers. Errors on any other tier.",
        [this](Title& self, int id) {
            BaronyTitle* barony = CastTitle<BaronyTitle>(&self, "a barony");
            m_Mod.GetTitleManager().ChangeBaronyProvinceId(barony, id);
            this->InvalidateMap();
        });

    SCRIPT_FN(title, "GetLocName", "title:GetLocName(lang) -> string",
        "Returns the title's localized name for a language (e.g. \"english\").",
        [](Title& self, const std::string& lang) { return self.GetLocName(lang); });

    SCRIPT_FN(title, "SetLocName", "title:SetLocName(lang, name)",
        "Sets the title's localized name for a language.",
        [](Title& self, const std::string& lang, const std::string& name) { self.SetLocName(lang, name); });

    SCRIPT_FN(title, "GetCulturalNames", "title:GetCulturalNames() -> table",
        "Returns the title's cultural names, as a culture name to title name table.",
        [this](Title& self) {
            sol::table names = m_State.create_table();
            for (const auto& [culture, name] : self.GetCulturalNames())
                names[culture] = name;
            return names;
        });

    SCRIPT_FN(title, "AddCulturalName", "title:AddCulturalName(culture, name)",
        "Adds (or replaces) the title's name for a culture.",
        [](Title& self, const std::string& culture, const std::string& name) {
            self.AddCulturalName(culture, name);
        });

    SCRIPT_FN(title, "RemoveCulturalName", "title:RemoveCulturalName(culture)",
        "Removes the title's name for a culture.",
        [](Title& self, const std::string& culture) { self.RemoveCulturalName(culture); });

    //////////////////////////////////////////////////////
    // Culture
    //////////////////////////////////////////////////////

    auto cultureType = m_State.new_usertype<Culture>("Culture", sol::no_constructor);
    auto culture = MakeBinder(m_Docs, cultureType, "Culture");

    SCRIPT_FN(culture, "GetName", "culture:GetName() -> string",
        "Returns the culture's name.",
        [](Culture& self) { return self.GetName(); });

    SCRIPT_FN(culture, "GetColor", "culture:GetColor() -> Color",
        "Returns the culture's color.",
        [](Culture& self) { return self.GetColor(); });

    SCRIPT_FN(culture, "SetColor", "culture:SetColor(color)",
        "Sets the culture's color.",
        [this](Culture& self, const sf::Color& color) {
            self.SetColor(color);
            this->InvalidateMap();
        });

    //////////////////////////////////////////////////////
    // Faith
    //////////////////////////////////////////////////////

    auto faithType = m_State.new_usertype<Faith>("Faith", sol::no_constructor);
    auto faith = MakeBinder(m_Docs, faithType, "Faith");

    SCRIPT_FN(faith, "GetName", "faith:GetName() -> string",
        "Returns the faith's name.",
        [](Faith& self) { return self.GetName(); });

    SCRIPT_FN(faith, "GetColor", "faith:GetColor() -> Color",
        "Returns the faith's color.",
        [](Faith& self) { return self.GetColor(); });

    SCRIPT_FN(faith, "SetColor", "faith:SetColor(color)",
        "Sets the faith's color.",
        [this](Faith& self, const sf::Color& color) {
            self.SetColor(color);
            this->InvalidateMap();
        });

    //////////////////////////////////////////////////////
    // Region
    //////////////////////////////////////////////////////

    auto regionType = m_State.new_usertype<Region>("Region", sol::no_constructor);
    auto region = MakeBinder(m_Docs, regionType, "Region");

    SCRIPT_FN(region, "GetName", "region:GetName() -> string",
        "Returns the region's name.",
        [](Region& self) { return self.GetName(); });

    SCRIPT_FN(region, "GetProvinces", "region:GetProvinces() -> Province[]",
        "Returns the provinces directly in the region.",
        [this](Region& self) { return ToArray(m_State, self.GetProvinces(), [](Province* p) { return p; }); });

    SCRIPT_FN(region, "GetCounties", "region:GetCounties() -> Title[]",
        "Returns the county titles directly in the region.",
        [this](Region& self) { return ToArray(m_State, self.GetCounties(), [](Title* t) { return t; }); });

    SCRIPT_FN(region, "GetDuchies", "region:GetDuchies() -> Title[]",
        "Returns the duchy titles directly in the region.",
        [this](Region& self) { return ToArray(m_State, self.GetDuchies(), [](Title* t) { return t; }); });

    SCRIPT_FN(region, "GetKingdoms", "region:GetKingdoms() -> Title[]",
        "Returns the kingdom titles directly in the region.",
        [this](Region& self) { return ToArray(m_State, self.GetKingdoms(), [](Title* t) { return t; }); });

    SCRIPT_FN(region, "GetRegions", "region:GetRegions() -> Region[]",
        "Returns the region's sub-regions.",
        [this](Region& self) { return ToArray(m_State, self.GetRegions(), [](Region* r) { return r; }); });

    SCRIPT_FN(region, "AddProvince", "region:AddProvince(province)",
        "Adds a province to the region.",
        [](Region& self, Province* province) { self.AddProvince(province); });

    SCRIPT_FN(region, "RemoveProvince", "region:RemoveProvince(province)",
        "Removes a province from the region.",
        [](Region& self, Province* province) { self.RemoveProvince(province); });

    SCRIPT_FN(region, "AddTitle", "region:AddTitle(title)",
        "Adds a title to the region, into the list matching its tier.",
        [](Region& self, Title* title) { self.AddTitle(title); });

    SCRIPT_FN(region, "RemoveTitle", "region:RemoveTitle(title)",
        "Removes a title from the region.",
        [](Region& self, Title* title) { self.RemoveTitle(title); });

    //////////////////////////////////////////////////////
    // Adjacency
    //////////////////////////////////////////////////////

    auto adjacencyType = m_State.new_usertype<Adjacency>("Adjacency", sol::no_constructor);
    auto adjacency = MakeBinder(m_Docs, adjacencyType, "Adjacency");

    SCRIPT_FN(adjacency, "GetFromId", "adjacency:GetFromId() -> integer",
        "Returns the id of the province the adjacency starts from.",
        [](Adjacency& self) { return self.GetFromId(); });

    SCRIPT_FN(adjacency, "GetToId", "adjacency:GetToId() -> integer",
        "Returns the id of the province the adjacency leads to.",
        [](Adjacency& self) { return self.GetToId(); });

    SCRIPT_FN(adjacency, "GetThroughId", "adjacency:GetThroughId() -> integer",
        "Returns the id of the province the adjacency passes through.",
        [](Adjacency& self) { return self.GetThroughId(); });

    SCRIPT_FN(adjacency, "SetThroughId", "adjacency:SetThroughId(id)",
        "Sets the province the adjacency passes through.",
        [](Adjacency& self, int id) { self.SetThroughId(id); });

    SCRIPT_FN(adjacency, "GetType", "adjacency:GetType() -> string",
        "Returns the adjacency's type (e.g. \"sea\", \"river_large\").",
        [](Adjacency& self) { return self.GetType(); });

    SCRIPT_FN(adjacency, "SetType", "adjacency:SetType(type)",
        "Sets the adjacency's type.",
        [](Adjacency& self, const std::string& type) { self.SetType(type); });

    SCRIPT_FN(adjacency, "GetComment", "adjacency:GetComment() -> string",
        "Returns the adjacency's comment.",
        [](Adjacency& self) { return self.GetComment(); });

    SCRIPT_FN(adjacency, "SetComment", "adjacency:SetComment(comment)",
        "Sets the adjacency's comment.",
        [](Adjacency& self, const std::string& comment) { self.SetComment(comment); });

    SCRIPT_FN(adjacency, "GetStart", "adjacency:GetStart() -> x, y",
        "Returns the adjacency line's start position, as two values.",
        [](Adjacency& self) {
            sf::Vector2u start = self.GetStart();
            return std::make_tuple(start.x, start.y);
        });

    SCRIPT_FN(adjacency, "GetStop", "adjacency:GetStop() -> x, y",
        "Returns the adjacency line's end position, as two values.",
        [](Adjacency& self) {
            sf::Vector2u stop = self.GetStop();
            return std::make_tuple(stop.x, stop.y);
        });
}

void Engine::BindManagers() {
    //////////////////////////////////////////////////////
    // mod
    //////////////////////////////////////////////////////

    auto mod = MakeBinder(m_Docs, m_State.create_named_table("mod"), "mod");

    SCRIPT_FN(mod, "GetRootDirectory", "mod.GetRootDirectory() -> string",
        "Returns the absolute path to the mod's root directory.",
        [this]() { return m_Mod.GetRootDirectory(); });

    SCRIPT_FN(mod, "HasMap", "mod.HasMap() -> boolean",
        "Checks whether the mod has a provinces map loaded.",
        [this]() { return m_Mod.HasMap(); });

    //////////////////////////////////////////////////////
    // provinces
    //////////////////////////////////////////////////////

    auto provinces = MakeBinder(m_Docs, m_State.create_named_table("provinces"), "provinces");

    SCRIPT_FN(provinces, "Count", "provinces.Count() -> integer",
        "Returns the total number of provinces.",
        [this]() { return m_Mod.GetProvinceManager().CountProvinces(); });

    SCRIPT_FN(provinces, "Has", "provinces.Has(id) -> boolean",
        "Checks whether a province with the given id exists.",
        [this](int id) { return m_Mod.GetProvinceManager().HasProvinceById(id); });

    SCRIPT_FN(provinces, "GetById", "provinces.GetById(id) -> Province|nil",
        "Returns the province with the given id, or nil.",
        [this](int id) { return m_Mod.GetProvinceManager().GetProvinceById(id); });

    SCRIPT_FN(provinces, "GetByColor", "provinces.GetByColor(color) -> Province|nil",
        "Returns the province with the given packed color integer, or nil.",
        [this](uint32_t color) { return m_Mod.GetProvinceManager().GetProvinceByColor(color); });

    SCRIPT_FN(provinces, "GetByPixel", "provinces.GetByPixel(x, y) -> Province|nil",
        "Returns the province covering a pixel of the provinces map, or nil.",
        [this](int x, int y) { return m_Mod.GetProvinceManager().GetProvinceByPixel(x, y); });

    SCRIPT_FN(provinces, "GetAll", "provinces.GetAll() -> Province[]",
        "Returns every province, ordered by id.",
        [this]() {
            return ToArray(m_State, m_Mod.GetProvinceManager().GetProvincesByIds(),
                [](const auto& pair) { return pair.second; });
        });

    SCRIPT_FN(provinces, "GetMaxId", "provinces.GetMaxId() -> integer",
        "Returns the highest province id in use, or 0 when there are no provinces.",
        [this]() { return m_Mod.GetProvinceManager().GetMaxProvinceId().value_or(0); });

    SCRIPT_FN(provinces, "Create", "provinces.Create(id, name, color) -> Province",
        "Creates a province and adds it to the mod. Errors if the id or color is already taken.",
        [this](int id, const std::string& name, const sf::Color& color) {
            ProvinceManager& manager = m_Mod.GetProvinceManager();

            if (manager.HasProvinceById(id))
                throw std::runtime_error(fmt::format("a province with id {} already exists", id));
            if (manager.HasProvinceByColor(color.toInteger()))
                throw std::runtime_error(fmt::format("a province with color {} already exists", color.toInteger()));

            manager.AddProvince(MakeUnique<Province>(id, color, name));
            this->InvalidateMap();
            return manager.GetProvinceById(id);
        });

    SCRIPT_FN(provinces, "Remove", "provinces.Remove(province)",
        "Removes a province from the mod.",
        [this](Province* province) {
            m_Mod.GetProvinceManager().RemoveProvince(province);
            this->InvalidateMap();
        });

    SCRIPT_FN(provinces, "RenameId", "provinces.RenameId(formerId, newId)",
        "Changes a province's id, keeping the manager's lookups in sync.",
        [this](int formerId, int newId) {
            m_Mod.GetProvinceManager().RenameProvinceId(formerId, newId);
        });

    SCRIPT_FN(provinces, "GetTerrainTypes", "provinces.GetTerrainTypes() -> string[]",
        "Returns the names of every terrain type defined by the mod.",
        [this]() {
            return ToArray(m_State, m_Mod.GetProvinceManager().GetTerrainTypes(),
                [](const auto& pair) { return pair.first; });
        });

    SCRIPT_FN(provinces, "GetHoldingTypes", "provinces.GetHoldingTypes() -> string[]",
        "Returns the names of every holding type defined by the mod.",
        [this]() {
            return ToArray(m_State, m_Mod.GetProvinceManager().GetHoldingTypes(),
                [](const auto& pair) { return pair.first; });
        });

    //////////////////////////////////////////////////////
    // titles
    //////////////////////////////////////////////////////

    auto titles = MakeBinder(m_Docs, m_State.create_named_table("titles"), "titles");

    SCRIPT_FN(titles, "Count", "titles.Count() -> integer",
        "Returns the total number of titles.",
        [this]() { return m_Mod.GetTitleManager().CountTitles(); });

    SCRIPT_FN(titles, "CountOfType", "titles.CountOfType(type) -> integer",
        "Returns the number of titles of a tier (e.g. TitleType.DUCHY).",
        [this](TitleType type) { return m_Mod.GetTitleManager().CountTitles(type); });

    SCRIPT_FN(titles, "Has", "titles.Has(name) -> boolean",
        "Checks whether a title with the given name exists.",
        [this](const std::string& name) { return m_Mod.GetTitleManager().HasTitle(name); });

    SCRIPT_FN(titles, "Get", "titles.Get(name) -> Title|nil",
        "Returns the title with the given name (e.g. \"c_paris\"), or nil.",
        [this](const std::string& name) { return m_Mod.GetTitleManager().GetTitle(name); });

    SCRIPT_FN(titles, "GetAll", "titles.GetAll() -> Title[]",
        "Returns every title, ordered by name.",
        [this]() {
            return ToArray(m_State, m_Mod.GetTitleManager().GetTitles(),
                [](const auto& pair) { return pair.second.get(); });
        });

    SCRIPT_FN(titles, "GetAllOfType", "titles.GetAllOfType(type) -> Title[]",
        "Returns every title of a tier (e.g. TitleType.KINGDOM).",
        [this](TitleType type) {
            auto& titlesByType = m_Mod.GetTitleManager().GetTitlesByType();

            auto it = titlesByType.find(type);
            if (it == titlesByType.end())
                return m_State.create_table();

            return ToArray(m_State, it->second, [](Title* title) { return title; });
        });

    SCRIPT_FN(titles, "GetBaronyByProvinceId", "titles.GetBaronyByProvinceId(id) -> Title|nil",
        "Returns the barony title covering a province id, or nil.",
        [this](int id) -> Title* { return m_Mod.GetTitleManager().GetBaronyByProvinceId(id); });

    SCRIPT_FN(titles, "Create", "titles.Create(name, type, color) -> Title",
        "Creates a title and adds it to the mod. The name must carry the tier's prefix (e.g. \"d_anjou\").",
        [this](const std::string& name, TitleType type, const sf::Color& color) {
            TitleManager& manager = m_Mod.GetTitleManager();

            if (manager.HasTitle(name))
                throw std::runtime_error(fmt::format("a title named '{}' already exists", name));
            if (!IsValidTitleName(name, type))
                throw std::runtime_error(fmt::format(
                    "'{}' is not a valid {} name (expected the '{}_' prefix)",
                    name, TitleTypeLabels[static_cast<int>(type)], GetTitlePrefixByType(type)
                ));

            manager.AddTitle(MakeTitle(type, name, color, false));
            this->InvalidateMap();
            return manager.GetTitle(name);
        });

    SCRIPT_FN(titles, "Remove", "titles.Remove(name)",
        "Removes the title with the given name from the mod.",
        [this](const std::string& name) {
            m_Mod.GetTitleManager().RemoveTitle(name);
            this->InvalidateMap();
        });

    SCRIPT_FN(titles, "Rename", "titles.Rename(formerName, newName)",
        "Renames a title, keeping the manager's lookups in sync.",
        [this](const std::string& formerName, const std::string& newName) {
            m_Mod.GetTitleManager().RenameTitle(formerName, newName);
        });

    //////////////////////////////////////////////////////
    // cultures
    //////////////////////////////////////////////////////

    auto cultures = MakeBinder(m_Docs, m_State.create_named_table("cultures"), "cultures");

    SCRIPT_FN(cultures, "Count", "cultures.Count() -> integer",
        "Returns the total number of cultures.",
        [this]() { return m_Mod.GetCultureManager().CountCultures(); });

    SCRIPT_FN(cultures, "Has", "cultures.Has(name) -> boolean",
        "Checks whether a culture with the given name exists.",
        [this](const std::string& name) { return m_Mod.GetCultureManager().HasCulture(name); });

    SCRIPT_FN(cultures, "Get", "cultures.Get(name) -> Culture|nil",
        "Returns the culture with the given name, or nil.",
        [this](const std::string& name) { return m_Mod.GetCultureManager().GetCulture(name); });

    SCRIPT_FN(cultures, "GetAll", "cultures.GetAll() -> Culture[]",
        "Returns every culture, ordered by name.",
        [this]() {
            std::vector<Culture*> sorted;
            for (const auto& [name, culture] : m_Mod.GetCultureManager().GetCultures())
                sorted.push_back(culture.get());

            std::sort(sorted.begin(), sorted.end(), [](Culture* a, Culture* b) {
                return a->GetName() < b->GetName();
            });
            return ToArray(m_State, sorted, [](Culture* culture) { return culture; });
        });

    //////////////////////////////////////////////////////
    // religions
    //////////////////////////////////////////////////////

    auto religions = MakeBinder(m_Docs, m_State.create_named_table("religions"), "religions");

    SCRIPT_FN(religions, "Count", "religions.Count() -> integer",
        "Returns the total number of faiths.",
        [this]() { return m_Mod.GetReligionManager().CountFaiths(); });

    SCRIPT_FN(religions, "Has", "religions.Has(name) -> boolean",
        "Checks whether a faith with the given name exists.",
        [this](const std::string& name) { return m_Mod.GetReligionManager().HasFaith(name); });

    SCRIPT_FN(religions, "Get", "religions.Get(name) -> Faith|nil",
        "Returns the faith with the given name, or nil.",
        [this](const std::string& name) { return m_Mod.GetReligionManager().GetFaith(name); });

    SCRIPT_FN(religions, "GetAll", "religions.GetAll() -> Faith[]",
        "Returns every faith, ordered by name.",
        [this]() {
            std::vector<Faith*> sorted;
            for (const auto& [name, faith] : m_Mod.GetReligionManager().GetFaiths())
                sorted.push_back(faith.get());

            std::sort(sorted.begin(), sorted.end(), [](Faith* a, Faith* b) {
                return a->GetName() < b->GetName();
            });
            return ToArray(m_State, sorted, [](Faith* faith) { return faith; });
        });

    //////////////////////////////////////////////////////
    // regions
    //////////////////////////////////////////////////////

    auto regions = MakeBinder(m_Docs, m_State.create_named_table("regions"), "regions");

    SCRIPT_FN(regions, "Count", "regions.Count() -> integer",
        "Returns the total number of geographical regions.",
        [this]() { return m_Mod.GetRegionManager().CountRegions(); });

    SCRIPT_FN(regions, "Has", "regions.Has(name) -> boolean",
        "Checks whether a region with the given name exists.",
        [this](const std::string& name) { return m_Mod.GetRegionManager().HasRegion(name); });

    SCRIPT_FN(regions, "Get", "regions.Get(name) -> Region|nil",
        "Returns the region with the given name, or nil.",
        [this](const std::string& name) { return m_Mod.GetRegionManager().GetRegion(name); });

    SCRIPT_FN(regions, "GetAll", "regions.GetAll() -> Region[]",
        "Returns every region, ordered by name.",
        [this]() {
            return ToArray(m_State, m_Mod.GetRegionManager().GetRegions(),
                [](const auto& pair) { return pair.second.get(); });
        });

    SCRIPT_FN(regions, "Remove", "regions.Remove(name)",
        "Removes the region with the given name from the mod.",
        [this](const std::string& name) { m_Mod.GetRegionManager().RemoveRegion(name); });

    //////////////////////////////////////////////////////
    // adjacencies
    //////////////////////////////////////////////////////

    auto adjacencies = MakeBinder(m_Docs, m_State.create_named_table("adjacencies"), "adjacencies");

    SCRIPT_FN(adjacencies, "Count", "adjacencies.Count() -> integer",
        "Returns the total number of adjacencies.",
        [this]() { return m_Mod.GetProvinceManager().GetAdjacencies().size(); });

    SCRIPT_FN(adjacencies, "Has", "adjacencies.Has(fromId, toId) -> boolean",
        "Checks whether an adjacency exists between two province ids.",
        [this](int fromId, int toId) { return m_Mod.GetProvinceManager().HasAdjacency(fromId, toId); });

    SCRIPT_FN(adjacencies, "Get", "adjacencies.Get(fromId, toId) -> Adjacency|nil",
        "Returns the adjacency between two province ids, or nil.",
        [this](int fromId, int toId) { return m_Mod.GetProvinceManager().GetAdjacencyByIds(fromId, toId); });

    SCRIPT_FN(adjacencies, "GetAll", "adjacencies.GetAll() -> Adjacency[]",
        "Returns every adjacency, ordered by province id pair.",
        [this]() {
            return ToArray(m_State, m_Mod.GetProvinceManager().GetAdjacencies(),
                [](const auto& pair) { return pair.second.get(); });
        });

    SCRIPT_FN(adjacencies, "Create", "adjacencies.Create(fromId, toId, type, throughId, comment) -> Adjacency",
        "Creates an adjacency between two provinces. Errors if one already exists.",
        [this](int fromId, int toId, const std::string& type, int throughId, const std::string& comment) {
            ProvinceManager& manager = m_Mod.GetProvinceManager();

            if (manager.HasAdjacency(fromId, toId))
                throw std::runtime_error(fmt::format("an adjacency between {} and {} already exists", fromId, toId));

            manager.AddAdjacency(MakeUnique<Adjacency>(
                fromId, toId, type, throughId, sf::Vector2u(-1, -1), sf::Vector2u(-1, -1), comment
            ));
            return manager.GetAdjacencyByIds(fromId, toId);
        });

    SCRIPT_FN(adjacencies, "Remove", "adjacencies.Remove(fromId, toId)",
        "Removes the adjacency between two province ids.",
        [this](int fromId, int toId) { m_Mod.GetProvinceManager().RemoveAdjacency(fromId, toId); });
}

void Engine::BindMap() {
    auto map = MakeBinder(m_Docs, m_State.create_named_table("map"), "map");

    SCRIPT_FN(map, "GetSize", "map.GetSize() -> width, height",
        "Returns the provinces map's size in pixels, as two values.",
        [this]() {
            sf::Vector2u size = m_Mod.GetProvinceManager().GetProvincesImage().getSize();
            return std::make_tuple(size.x, size.y);
        });

    SCRIPT_FN(map, "GetProvinceAt", "map.GetProvinceAt(x, y) -> Province|nil",
        "Returns the province covering a pixel of the provinces map, or nil.",
        [this](int x, int y) { return m_Mod.GetProvinceManager().GetProvinceByPixel(x, y); });

    SCRIPT_FN(map, "GetColorAt", "map.GetColorAt(x, y) -> Color",
        "Returns the raw color of a pixel of the provinces map.",
        [this](unsigned int x, unsigned int y) {
            const sf::Image& image = m_Mod.GetProvinceManager().GetProvincesImage();

            sf::Vector2u size = image.getSize();
            if (x >= size.x || y >= size.y)
                throw std::runtime_error(fmt::format("pixel ({}, {}) is outside the map", x, y));

            return image.getPixel({ x, y });
        });

    SCRIPT_FN(map, "GetHeightAt", "map.GetHeightAt(x, y) -> integer",
        "Returns the heightmap elevation at a pixel, in [0, 255].",
        [this](unsigned int x, unsigned int y) {
            const sf::Image& image = m_Mod.GetProvinceManager().GetHeightmapImage();

            sf::Vector2u size = image.getSize();
            if (x >= size.x || y >= size.y)
                throw std::runtime_error(fmt::format("pixel ({}, {}) is outside the heightmap", x, y));

            return static_cast<int>(image.getPixel({ x, y }).r);
        });

    SCRIPT_FN(map, "Invalidate", "map.Invalidate()",
        "Marks the map as needing a redraw, so the editor rebuilds its textures once the script ends.",
        [this]() { this->InvalidateMap(); });

    SCRIPT_FN(map, "RebuildIndex", "map.RebuildIndex()",
        "Rebuilds the province render index. Needed after adding, removing or recoloring provinces.",
        [this]() {
            m_Mod.GetProvinceManager().BuildProvinceIndex();
            this->InvalidateMap();
        });
}

}
