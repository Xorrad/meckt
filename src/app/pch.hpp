// Core precompiled header
#include "core/pch.hpp"

// Forward declarations:
enum class Textures : int;
enum class Fonts : int;
enum class Shaders : int;

// Custom backward::SignalHandling to print stacktrace to file.
class SignalHandler;

class App;
class Configuration;

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

// Internal headers
#include "app/Configuration.hpp"

#include "app/menu/selection/SelectionCallbackResult.hpp"
#include "app/menu/tab/Tab.hpp"