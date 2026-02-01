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
#include <type_traits> // std::underlying_type_t
#include <thread> // std::thread

#ifdef _WIN32
typedef unsigned long ulong;
typedef unsigned short ushort;
typedef unsigned int uint;
#endif

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <Jomini.hpp>

// Custom backward::SignalHandling to print stacktrace to file.
class SignalHandler;

class App;

class Mod;
class Culture;
class Religion;
class Province;
class Region;
class Title;
class HighTitle;
class BaronyTitle;
class CountyTitle;
class DuchyTitle;
class KingdomTitle;
class EmpireTitle;

class Menu;
class HomeMenu;
class LoadingMenu;
class NewModMenu;
class EditorMenu;

class Tab;
class TitlesTab;
class RegionsTab;
class ProvincesTab;
class PropertiesTab;
class CulturalNamesTab;
class LogTab;

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
#include "app/Configuration.hpp"
#include "app/mod/LoadingState.hpp"

#include "app/map/TitleType.hpp"
#include "app/map/MapMode.hpp"
#include "app/map/Province.hpp"
#include "app/menu/selection/SelectionCallbackResult.hpp"
#include "app/menu/tab/Tab.hpp"