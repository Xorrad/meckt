#include <iostream>
#include <unordered_map>
#include <memory>
#include <string>
#include <math.h>
#include <vector>
#include <variant>
#include <deque>
#include <set>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "util/Ptr.hpp"
#include "util/Logger.hpp"
#include "util/String.hpp"
#include "util/File.hpp"
#include "util/Date.hpp"
#include "app/Configuration.hpp"

#include "app/map/MapMode.hpp"
#include "app/map/TitleType.hpp"
#include "app/map/TerrainType.hpp"

// Custom backward::SignalHandling to print stacktrace to file.
class SignalHandler;

class App;

namespace Parser {
    class Token;
    class Node;
    class AbstractValueHolder;
    class NodeHolder;
    class LeafHolder;
}

class Mod;
class Province;
class Title;
class HighTitle;
class BaronyTitle;
class CountyTitle;
class DuchyTitle;
class KingdomTitle;
class EmpireTitle;

class Menu;
class HomeMenu;
class EditingMenu;