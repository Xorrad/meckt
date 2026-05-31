// Forward declarations
enum class ClimateType : int;
enum class ProvinceFlags : int;
enum class TitleType : int;

class Mod;

class ProvinceManager;
class RegionManager;
class TitleManager;
class CultureManager;
class ReligionManager;

class HoldingType;
class Province;
class TerrainType;
class Region;
class Title;
class HighTitle;
class BaronyTitle;
class CountyTitle;
class DuchyTitle;
class KingdomTitle;
class EmpireTitle;
class HegemonyTitle;

class Culture;
class Faith;

// Standard headers
#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <queue>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <math.h>
#include <variant>
#include <random>
#include <memory> // std::shared_ptr, std::make_shared
#include <algorithm> // std::sort, std::find, std::remove, std::min_element, std::max_element
#include <ranges> // std::views, std::ranges, std::reverse
#include <functional> // lambda functions, std::function, std::reference_wrapper
#include <span> // std::span
#include <type_traits> // std::underlying_type_t, std::is_same_v
#include <thread> // std::thread

// SFML headers
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

// Internal headers
#include <Jomini.hpp>

#include "util/Ptr.hpp"
#include "util/Logger.hpp"
#include "util/String.hpp"
#include "util/Math.hpp"
#include "util/File.hpp"
#include "util/Color.hpp"
#include "util/Date.hpp"
#include "util/ScopedString.hpp"
#include "util/Image.hpp"
#include "util/OrderedMap.hpp"

#include "provinces/ClimateType.hpp"
#include "provinces/HoldingType.hpp"
#include "provinces/ProvinceFlags.hpp"
#include "provinces/TerrainType.hpp"
#include "titles/TitleType.hpp"

#include "mod/LoadingState.hpp"
#include "mod/Paths.hpp"