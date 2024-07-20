#include <iostream>
#include <unordered_map>
#include <memory>
#include <string>
#include <math.h>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "util/Ptr.hpp"
#include "util/Logger.hpp"
#include "app/Configuration.hpp"

// Custom backward::SignalHandling to print stacktrace to file.
class SignalHandler;

class App;
class Mod;
class Menu;
class HomeMenu;
class EditingMenu;